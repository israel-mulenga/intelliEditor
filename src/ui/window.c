#include "ui/window.h"
#include "ui/editor.h"
#include "ui/sidebar.h"
#include "ui/toolbar.h"
#include "ui/callbacks.h"
#include "config/config_parser.h"
#include "editor/file_manager.h"
#include "llm/llm_client.h"
#include "editor/gap_buffer.h"
#include "nlp/nlp_highlighter.h"
#include <hunspell/hunspell.h>


/* =========================================================
   PROTOTYPES INTERNES (usage uniquement dans ce fichier)
   ========================================================= */
static void setup_css(const char *theme);
static gboolean autosave_tick(gpointer user_data);
static void init_hunspell(AppWidgets *app_widgets);
static void cleanup_app_widgets(GtkWidget *widget, gpointer data);
static GtkWidget* create_editor_page(AppWidgets *app_widgets, GtkAccelGroup *accel_group);
static gboolean draw_horizontal_ruler(
    GtkWidget *widget,
    cairo_t *cr,
    gpointer data
);
static gboolean draw_vertical_ruler(
    GtkWidget *widget,
    cairo_t *cr,
    gpointer data
);

/* =========================================================
   FONCTION PRINCIPALE : création fenêtre
   ========================================================= */
void app_autosave_reschedule(AppWidgets *app) {
    if (!app) {
        return;
    }

    if (app->autosave_source_id != 0) {
        g_source_remove(app->autosave_source_id);
        app->autosave_source_id = 0;
    }

    if (app->autosave_interval_sec == 0 || app->current_file_path == NULL) {
        return;
    }

    app->autosave_source_id = g_timeout_add_seconds(
        app->autosave_interval_sec,
        autosave_tick,
        app
    );
}

static gboolean autosave_tick(gpointer user_data) {
    AppWidgets *app = (AppWidgets *)user_data;

    if (!app || !app->gb || !app->current_file_path) {
        return G_SOURCE_CONTINUE;
    }

    if (!gap_buffer_save_to_file(app->gb, app->current_file_path)) {
        g_printerr("Autosave failed for: %s\n", app->current_file_path);
    }

    return G_SOURCE_CONTINUE;
}

void create_main_window(GtkApplication *app, gpointer user_data) {
    typedef struct {
        GapBuffer *gb;
        AppConfig config;
    } AppStartupData;

    AppStartupData *startup = (AppStartupData *)user_data;
    GapBuffer *gb = startup ? startup->gb : NULL;

    GtkWidget *window;
    GtkWidget *stack;
    GtkAccelGroup *accel_group;

    /* Allocation structure globale */
    AppWidgets *app_widgets = g_new0(AppWidgets, 1);

    app_widgets->gb = gb;
    app_widgets->app = app;
    app_widgets->hun_en = NULL;
    app_widgets->hun_fr = NULL;
    app_widgets->autosave_source_id = 0;
    app_widgets->autosave_interval_sec = 0;

    if (startup) {
        app_widgets->config = startup->config;
        app_widgets->autosave_interval_sec = startup->config.autosave_interval_sec;
    } else {
        app_config_set_defaults(&app_widgets->config);
    }

    init_hunspell(app_widgets);

    /* Chargement du style CSS */
    setup_css(app_widgets->config.theme);


    /* ================= WINDOW ================= */
    window = gtk_application_window_new(app);
    app_widgets->window = window;
    gtk_window_set_title(GTK_WINDOW(window), "IntelliEditor");
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 650);
    gtk_widget_set_name(window, "app-window");

    accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

    /* ================= STACK (pages) ================= */
    stack = gtk_stack_new();
    gtk_container_add(GTK_CONTAINER(window), stack);
    app_widgets->stack = stack;

    GtkWidget *welcome_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_set_halign(welcome_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(welcome_box, GTK_ALIGN_CENTER);

    GtkWidget *welcome_label = gtk_label_new("Bienvenue sur INTELLIEDITOR");
    gtk_widget_set_name(welcome_label, "welcome-label");
    gtk_box_pack_start(GTK_BOX(welcome_box), welcome_label, FALSE, FALSE, 0);

    GtkWidget *open_button = gtk_button_new_with_label("Ouvrir l'éditeur");
    gtk_widget_set_name(open_button, "welcome-button");
    g_signal_connect(open_button, "clicked", G_CALLBACK(on_start_clicked), stack);
    gtk_box_pack_start(GTK_BOX(welcome_box), open_button, FALSE, FALSE, 0);

    gtk_stack_add_named(GTK_STACK(stack), welcome_box, "welcome");
    gtk_stack_add_named(GTK_STACK(stack), create_editor_page(app_widgets, accel_group), "editor");

    if (!llm_client_server_up() && app_widgets->statusbar) {
        gtk_statusbar_push(GTK_STATUSBAR(app_widgets->statusbar), 0,
            "LLM server unreachable (check config.ini host/port)");
    }

    gtk_stack_set_transition_type(GTK_STACK(stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "welcome");

    /* ================= SIGNAL CLEANUP ================= */
    g_signal_connect(window, "destroy",
        G_CALLBACK(cleanup_app_widgets), app_widgets);

    gtk_widget_show_all(window);
}

/* =========================================================
   PAGE ÉDITEUR (VERSION PROPRE WORD-LIKE)
   ========================================================= */
GtkWidget* create_new_tab(AppWidgets *app_widgets, const gchar *title)
{
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);

    GtkWidget *editor = create_editor();

    gtk_container_add(GTK_CONTAINER(scroll), editor);

    GtkWidget *tab_label = gtk_label_new(title);

    gtk_notebook_append_page(
        GTK_NOTEBOOK(app_widgets->notebook),
        scroll,
        tab_label
    );

    gtk_widget_show_all(scroll);

    return scroll;
}

void append_new_page_to_current_document(AppWidgets *app_widgets)
{
    if (!app_widgets || !app_widgets->notebook) {
        return;
    }

    GtkNotebook *notebook = GTK_NOTEBOOK(app_widgets->notebook);
    gint page_index = gtk_notebook_get_current_page(notebook);
    if (page_index < 0) {
        return;
    }

    GtkWidget *scroll = gtk_notebook_get_nth_page(notebook, page_index);
    if (!scroll) {
        return;
    }

    GtkWidget *child = gtk_bin_get_child(GTK_BIN(scroll));
    GtkWidget *center_box = NULL;

    /* If scrolled window wraps the child in a viewport, unwrap it. */
    if (GTK_IS_VIEWPORT(child)) {
        center_box = gtk_bin_get_child(GTK_BIN(child));
    } else if (GTK_IS_BOX(child)) {
        center_box = child;
    }

    if (!center_box || !GTK_IS_BOX(center_box)) {
        return;
    }

    /* Créer une nouvelle page A4 */
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(page, "a4-page");
    gtk_widget_set_size_request(page, 794, 1123);

    /* Rulers */
    GtkWidget *horizontal_ruler = gtk_drawing_area_new();
    gtk_widget_set_name(horizontal_ruler, "horizontal-ruler");
    gtk_widget_set_size_request(horizontal_ruler, 794, 30);
    g_signal_connect(horizontal_ruler, "draw", G_CALLBACK(draw_horizontal_ruler), NULL);

    GtkWidget *vertical_ruler = gtk_drawing_area_new();
    gtk_widget_set_name(vertical_ruler, "vertical-ruler");
    gtk_widget_set_size_request(vertical_ruler, 30, -1);
    g_signal_connect(vertical_ruler, "draw", G_CALLBACK(draw_vertical_ruler), NULL);

    /* Editor */
    GtkWidget *editor = create_editor();
    gtk_widget_set_name(editor, "editor-textview");

    GtkWidget *editor_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(editor_row), vertical_ruler, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(editor_row), editor, TRUE, TRUE, 0);

    /* Ajouter rulers + editor dans la page */
    gtk_box_pack_start(GTK_BOX(page), horizontal_ruler, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(page), editor_row, TRUE, TRUE, 0);

    gtk_widget_set_margin_top(editor, 96);
    gtk_widget_set_margin_bottom(editor, 96);
    gtk_widget_set_margin_start(editor, 96);
    gtk_widget_set_margin_end(editor, 96);

    gtk_widget_set_hexpand(editor, TRUE);
    gtk_widget_set_vexpand(editor, FALSE);

    /* Insérer la nouvelle page sous les autres */
    gtk_box_pack_start(GTK_BOX(center_box), page, FALSE, FALSE, 40);
    gtk_widget_show_all(page);
}
   
GtkWidget* create_editor_page(AppWidgets *app_widgets, GtkAccelGroup *accel_group) {

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    /* ================= TOOLBAR ================= */
    GtkWidget *toolbar = create_toolbar(app_widgets, accel_group);
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);

    /* ================= NOTEBOOK ================= */
    GtkWidget *notebook = gtk_notebook_new();

    gtk_widget_set_hexpand(notebook, TRUE);
    gtk_widget_set_vexpand(notebook, TRUE);

    app_widgets->notebook = notebook;

    /* ================= ZONE DOCUMENT + SIDEBAR (cahier) ================= */
    GtkWidget *content_paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    app_widgets->sidebar = create_sidebar();

    const char *rules_path = rules_default_file_path();
    if (rules_path) {
        app_widgets->rules_file_path = g_strdup(rules_path);
        app_widgets->ruleset = ruleset_load(rules_path);
        if (app_widgets->ruleset) {
            sidebar_set_rules_file(app_widgets->sidebar, rules_path);
            sidebar_bind_ruleset(app_widgets->sidebar, app_widgets->ruleset);
        }
    }

    gtk_paned_pack1(GTK_PANED(content_paned), notebook, TRUE, TRUE);
    gtk_paned_pack2(GTK_PANED(content_paned), app_widgets->sidebar, FALSE, FALSE);
    gtk_paned_set_wide_handle(GTK_PANED(content_paned), TRUE);
    gtk_paned_set_position(GTK_PANED(content_paned), 820);

    gtk_box_pack_start(GTK_BOX(vbox), content_paned, TRUE, TRUE, 0);

    /* ================= CONTAINER CENTRÉ ================= */
    GtkWidget *center_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(center_box, GTK_ALIGN_CENTER);

    /* ================= PAGE A4 ================= */
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(page, "a4-page");

    /* Taille fixe A4 : 794x1123 pixels à 96 DPI */
    gtk_widget_set_size_request(page, 794, 1123);

    /* ================= RULERS ================= */

  /* ================= HORIZONTAL RULER ================= */

    GtkWidget *horizontal_ruler = gtk_drawing_area_new();

    gtk_widget_set_name(horizontal_ruler, "horizontal-ruler");

    gtk_widget_set_size_request(horizontal_ruler, 794, 30);

    g_signal_connect(
        horizontal_ruler,
        "draw",
        G_CALLBACK(draw_horizontal_ruler),
        NULL
    );

    app_widgets->horizontal_ruler = horizontal_ruler;

    /* ================= VERTICAL RULER ================= */

    GtkWidget *vertical_ruler = gtk_drawing_area_new();

    gtk_widget_set_name(vertical_ruler, "vertical-ruler");

    gtk_widget_set_size_request(vertical_ruler, 30, -1);

    g_signal_connect(
        vertical_ruler,
        "draw",
        G_CALLBACK(draw_vertical_ruler),
        NULL
    );

    app_widgets->vertical_ruler = vertical_ruler;

    /* ================= EDITOR ================= */

    GtkWidget *editor = create_editor();

    gtk_widget_set_name(editor, "editor-textview");

    app_widgets->editor_view = editor;

    app_widgets->editor_buffer = GTK_SOURCE_BUFFER(
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor))
    );
    /* ================= NLP HIGHLIGHTER ================= */

    app_widgets->highlighter =
        nlp_highlighter_create(GTK_SOURCE_VIEW(editor));

    g_signal_connect(
        app_widgets->editor_buffer,
        "changed",
        G_CALLBACK(nlp_highlighter_on_text_changed),
        app_widgets->highlighter
    );

    nlp_highlighter_run_now(app_widgets->highlighter);

        /* Connect text buffer handlers to keep gap buffer in sync */
    app_widgets->insert_handler_id = g_signal_connect(app_widgets->editor_buffer,
                    "insert-text", G_CALLBACK(on_text_inserted), app_widgets);
    app_widgets->delete_handler_id = g_signal_connect(app_widgets->editor_buffer,
                    "delete-range", G_CALLBACK(on_text_deleted), app_widgets);

    /* ================= RÈGLE + ÉDITEUR ================= */

    GtkWidget *editor_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    gtk_box_pack_start(
        GTK_BOX(editor_row),
        vertical_ruler,
        FALSE,
        FALSE,
        0
    );

    gtk_box_pack_start(
        GTK_BOX(editor_row),
        editor,
        TRUE,
        TRUE,
        0
    );

    /* ================= AJOUT DANS PAGE ================= */

    gtk_box_pack_start(
        GTK_BOX(page),
        horizontal_ruler,
        FALSE,
        FALSE,
        0
    );

    gtk_box_pack_start(
        GTK_BOX(page),
        editor_row,
        TRUE,
        TRUE,
        0
    );

    /* Marges Word */
    gtk_widget_set_margin_top(editor, 96);
    gtk_widget_set_margin_bottom(editor, 96);
    gtk_widget_set_margin_start(editor, 96);
    gtk_widget_set_margin_end(editor, 96);

    gtk_widget_set_hexpand(editor, TRUE);
    gtk_widget_set_vexpand(editor, FALSE);

    /* Assemblage */
    gtk_box_pack_start(GTK_BOX(center_box), page, FALSE, FALSE, 40);
    gtk_container_add(GTK_CONTAINER(scroll), center_box);

    GtkWidget *tab_label = gtk_label_new("Document 1");

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook),scroll,tab_label);

    /* ================= STATUSBAR ================= */
    GtkWidget *statusbar = gtk_statusbar_new();
    gtk_widget_set_name(statusbar, "statusbar");

    app_widgets->statusbar = statusbar;

    gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, FALSE, 0);

    gtk_widget_grab_focus(editor);

    return vbox;
}
/* =========================================================
   CSS (STYLE GLOBAL)
   ========================================================= */
static void setup_css(const char *theme) {

    GtkCssProvider *provider = gtk_css_provider_new();
    gboolean dark = theme && g_ascii_strcasecmp(theme, "dark") == 0;

    const gchar *window_bg = dark ? "#2b2b2b" : "#e8e8e8";
    const gchar *window_fg = dark ? "#e8e8e8" : "#1e1e1e";
    const gchar *toolbar_bg = dark ? "#333333" : "#f7f7f7";
    const gchar *page_bg = dark ? "#1e1e1e" : "#ffffff";
    const gchar *page_fg = dark ? "#f0f0f0" : "#1e1e1e";
    const gchar *status_bg = dark ? "#2f2f2f" : "#f0f0f0";
    const gchar *status_fg = dark ? "#bbbbbb" : "#666666";

    gchar *css = g_strdup_printf(
        /* ===== FENÊTRE ===== */
        "window#app-window {"
        "    background-color: %s;"
        "    color: %s;"
        "}"

        /* ===== TOOLBAR ===== */
        "#toolbar {"
        "    background-color: %s;"
        "    border-bottom: 1px solid #d0d0d0;"
        "    padding: 8px 12px;"
        "}"
        
        "button#toolbar-button {"
        "    background-color: %s;"
        "    color: %s;"
        "    border: 1px solid #bfbfbf;"
        "    border-radius: 3px;"
        "    padding: 6px 12px;"
        "    font-size: 13px;"
        "}"
        
        "button#toolbar-button:hover {"
        "    background-color: #e8e8e8;"
        "    border: 1px solid #9e9e9e;"
        "}"

        /* ===== DOCUMENT CONTAINER (feuille) ===== */
        "#a4-page {"
        "    background-color: %s;"
        "    border: 1px solid #cfcfcf;"
        "    box-shadow: 0 10px 30px rgba(0,0,0,0.15);"
        "}"

        /* ===== ZONE D'ÉDITION (FEUILLE) ===== */
        "#editor-textview {"
        "    background-color: %s;"
        "    color: %s;"
        "    padding: 0px;"
        "    border: none;"
        "    font-family: 'Calibri', 'Arial', sans-serif;"
        "    font-size: 14px;"
        "    line-height: 1.5;"
        "}"

        "textview text {"
        "    background-color: %s;"
        "    color: %s;"
        "    caret-color: #111111;"
        "    selection-background-color: #0078d4;"
        "    selection-color: #ffffff;"
        "}"

        "#horizontal-ruler {"
        "    background-color: #f3f3f3;"
        "    border-bottom: 1px solid #cdcdcd;"
        "}"

        "#vertical-ruler {"
        "    background-color: #f3f3f3;"
        "    border-right: 1px solid #cdcdcd;"
        "}"

        /* ===== STATUSBAR ===== */
        "#statusbar {"
        "    background-color: %s;"
        "    color: %s;"
        "    border-top: 1px solid #d0d0d0;"
        "    padding: 4px 8px;"
        "    font-size: 12px;"
        "}"

        /* ===== SIDEBAR ===== */
        "#sidebar {"
        "    background-color: #f9f9f9;"
        "    border-left: 1px solid #eaeaea;"
        "    padding: 12px;"
        "}"

        /* ===== BOUTONS SIMPLES (STYLE ÉDITEUR) ===== */
        "button {"
        "    background-color: #ffffff;"
        "    color: #6f51bb;"
        "    border: 1px solid #dcdcdc;"
        "    border-radius: 6px;"
        "    padding: 6px 12px;"
        "}"

        "button:hover {"
        "    background-color: #f0f0f0;"
        "}"

        "button:active {"
        "    background-color: #e6e6e6;"
        "}"

        /* ===== STATUS BAR ===== */
        "statusbar#statusbar {"
        "    background-color: %s;"
        "    color: %s;"
        "    border-top: 1px solid #e6e6e6;"
        "    padding: 6px 10px;"
        "    font-size: 12px;"
        "}"

        "#horizontal-ruler {"
        "   background-color: #f8fafc;"
        "   border-bottom: 1px solid #cbd5e1;"
        "}"

        "#vertical-ruler {"
        "   background-color: #f8fafc;"
        "   border-right: 1px solid #cbd5e1;"
        "}",
        window_bg, window_fg,
        toolbar_bg, toolbar_bg, page_fg,
        page_bg, page_bg, page_fg,
        page_bg, page_fg,
        status_bg, status_fg,
        status_bg, status_fg
    );

    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    g_free(css);

    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
}

/* =========================================================
   CLEANUP
   ========================================================= */
static void cleanup_app_widgets(GtkWidget *widget, gpointer data) {
    (void)widget; // unused

    AppWidgets *app = (AppWidgets *)data;

    if (app->autosave_source_id != 0) {
        g_source_remove(app->autosave_source_id);
        app->autosave_source_id = 0;
    }

    if (app->hun_en) {
        Hunspell_destroy(app->hun_en);
    }
    if (app->hun_fr) {
        Hunspell_destroy(app->hun_fr);
    }

    if (app->gb) {
        gap_buffer_destroy(app->gb);
    }

    g_free(app->current_file_path);
    g_free(app->rules_file_path);
    ruleset_free(app->ruleset);

    if (app->highlighter) {
    nlp_highlighter_destroy(app->highlighter);
    }

    g_free(app);
}

static void init_hunspell(AppWidgets *app_widgets) {
    const gchar *en_aff = "/usr/share/hunspell/en_US.aff";
    const gchar *en_dic = "/usr/share/hunspell/en_US.dic";
    const gchar *fr_aff = "/usr/share/hunspell/fr_FR.aff";
    const gchar *fr_dic = "/usr/share/hunspell/fr_FR.dic";

    if (g_file_test(en_aff, G_FILE_TEST_EXISTS) && g_file_test(en_dic, G_FILE_TEST_EXISTS)) {
        app_widgets->hun_en = Hunspell_create(en_aff, en_dic);
    }
    if (g_file_test(fr_aff, G_FILE_TEST_EXISTS) && g_file_test(fr_dic, G_FILE_TEST_EXISTS)) {
        app_widgets->hun_fr = Hunspell_create(fr_aff, fr_dic);
    }
}

static gboolean draw_horizontal_ruler(
    GtkWidget *widget,
    cairo_t *cr,
    gpointer data)
{
    (void)data;

    GtkAllocation alloc;
    gtk_widget_get_allocation(widget, &alloc);

    int width = alloc.width;
    int height = alloc.height;

    /* fond */
    cairo_set_source_rgb(cr, 0.95, 0.95, 0.95);
    cairo_paint(cr);

    /* couleur des traits */
    cairo_set_source_rgb(cr, 0.15, 0.15, 0.15);

    cairo_set_line_width(cr, 1);

    int cm_spacing = 50;

    for (int i = 0; i < width; i += 10)
    {
        int line_height = 8;

        /* grande graduation */
        if (i % cm_spacing == 0)
            line_height = 22;

        /* moyenne graduation */
        else if (i % 20 == 0)
            line_height = 15;

        cairo_move_to(cr, i, height);
        cairo_line_to(cr, i, height - line_height);

        cairo_stroke(cr);

        /* numéros */
        if (i % cm_spacing == 0)
        {
            char number[10];

            sprintf(number, "%d", i / cm_spacing);

            cairo_move_to(cr, i + 2, 12);

            cairo_set_font_size(cr, 11);

            cairo_show_text(cr, number);
        }
    }

    return FALSE;
}

static gboolean draw_vertical_ruler(
    GtkWidget *widget,
    cairo_t *cr,
    gpointer data)
{
    (void)data;

    GtkAllocation alloc;

    gtk_widget_get_allocation(widget, &alloc);

    int width = alloc.width;
    int height = alloc.height;

    cairo_set_source_rgb(cr, 0.95, 0.95, 0.95);

    cairo_paint(cr);

    cairo_set_source_rgb(cr, 0.15, 0.15, 0.15);

    int cm_spacing = 50;

    for (int i = 0; i < height; i += 10)
    {
        int line_width = 8;

        if (i % cm_spacing == 0)
            line_width = 20;

        else if (i % 20 == 0)
            line_width = 14;

        cairo_move_to(cr, width, i);

        cairo_line_to(cr, width - line_width, i);

        cairo_stroke(cr);

        if (i % cm_spacing == 0)
        {
            char number[10];

            sprintf(number, "%d", i / cm_spacing);

            cairo_save(cr);

            cairo_translate(cr, 10, i + 10);

            cairo_rotate(cr, -1.5708);

            cairo_set_font_size(cr, 10);

            cairo_show_text(cr, number);

            cairo_restore(cr);
        }
    }

    return FALSE;
}