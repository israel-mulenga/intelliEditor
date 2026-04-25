#include "ui/window.h"
#include "ui/editor.h"
#include "ui/sidebar.h"
#include "ui/toolbar.h"
#include "ui/callbacks.h"
#include "editor/gap_buffer.h"
#include <hunspell/hunspell.h>

/* =========================================================
   PROTOTYPES INTERNES (usage uniquement dans ce fichier)
   ========================================================= */
static void setup_css(void);
static void init_hunspell(AppWidgets *app_widgets);
static void cleanup_app_widgets(GtkWidget *widget, gpointer data);
static GtkWidget* create_editor_page(AppWidgets *app_widgets);

/* =========================================================
   FONCTION PRINCIPALE : création fenêtre
   ========================================================= */
void create_main_window(GtkApplication *app, gpointer user_data) {
    GapBuffer *gb = (GapBuffer *)user_data; 

    GtkWidget *window;
    GtkWidget *stack;

    /* Allocation structure globale */
    AppWidgets *app_widgets = g_new0(AppWidgets, 1);

    app_widgets->gb = gb;
    app_widgets->app = app;
    app_widgets->hun_en = NULL;
    app_widgets->hun_fr = NULL;
    init_hunspell(app_widgets);

    /* Chargement du style CSS */
    setup_css();


    /* ================= WINDOW ================= */
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "IntelliEditor");
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 650);
    gtk_widget_set_name(window, "app-window");

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
    gtk_stack_add_named(GTK_STACK(stack), create_editor_page(app_widgets), "editor");

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
GtkWidget* create_editor_page(AppWidgets *app_widgets) {

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    /* ================= TOOLBAR ================= */
    GtkWidget *toolbar = create_toolbar(app_widgets);
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);

    /* ================= SCROLL (ZONE DOCUMENT) ================= */
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_name(scroll, "document-scroll");

    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    gtk_widget_set_hexpand(scroll, TRUE);
    gtk_widget_set_vexpand(scroll, TRUE);

    /* ================= CONTAINER CENTRÉ ================= */
    GtkWidget *center_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(center_box, GTK_ALIGN_CENTER);

    /* ================= PAGE A4 ================= */
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(page, "a4-page");

    /* Taille A4 réaliste */
    gtk_widget_set_size_request(page, 794, 1123);

    /* ================= EDITOR ================= */
    GtkWidget *editor = create_editor();
    gtk_widget_set_name(editor, "editor-textview");

    app_widgets->editor_buffer = GTK_SOURCE_BUFFER(
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor))
    );
    g_signal_connect(app_widgets->editor_buffer, "insert-text",
                    G_CALLBACK(on_text_inserted), app_widgets);

    g_signal_connect(app_widgets->editor_buffer, "delete-range",
                    G_CALLBACK(on_text_deleted), app_widgets);

    /* Marges Word */
    gtk_widget_set_margin_top(editor, 96);
    gtk_widget_set_margin_bottom(editor, 96);
    gtk_widget_set_margin_start(editor, 96);
    gtk_widget_set_margin_end(editor, 96);

    gtk_widget_set_hexpand(editor, TRUE);
    gtk_widget_set_vexpand(editor, TRUE);

    /* Assemblage */
    gtk_box_pack_start(GTK_BOX(page), editor, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(center_box), page, FALSE, FALSE, 40);
    gtk_container_add(GTK_CONTAINER(scroll), center_box);

    gtk_box_pack_start(GTK_BOX(vbox), scroll, TRUE, TRUE, 0);

    /* ================= STATUSBAR ================= */
    GtkWidget *statusbar = gtk_statusbar_new();
    gtk_widget_set_name(statusbar, "statusbar");

    app_widgets->statusbar = statusbar;

    gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, FALSE, 0);

    return vbox;
}
/* =========================================================
   CSS (STYLE GLOBAL)
   ========================================================= */
static void setup_css(void) {

    GtkCssProvider *provider = gtk_css_provider_new();

    const gchar *css =
        /* ===== FENÊTRE ===== */
        "window#app-window {"
        "    background-color: #e8e8e8;"
        "    color: #1e1e1e;"
        "}"

        /* ===== TOOLBAR ===== */
        "#toolbar {"
        "    background-color: #f7f7f7;"
        "    border-bottom: 1px solid #d0d0d0;"
        "    padding: 8px 12px;"
        "}"
        
        "button#toolbar-button {"
        "    background-color: #f0f0f0;"
        "    color: #1e1e1e;"
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
        "    background-color: #ffffff;"
        "    border: 1px solid #cfcfcf;"
        "    box-shadow: 0 10px 30px rgba(0,0,0,0.15);"
        "}"

        /* ===== ZONE D'ÉDITION (FEUILLE) ===== */
        "#editor-textview {"
        "    background-color: #ffffff;"
        "    color: #1e1e1e;"
        "    padding: 0px;"
        "    border: none;"
        "    font-family: 'Calibri', 'Arial', sans-serif;"
        "    font-size: 14px;"
        "    line-height: 1.5;"
        "}"

        "textview text {"
        "    background-color: #ffffff;"
        "    color: #1e1e1e;"
        "    selection-background-color: #0078d4;"
        "    selection-color: #ffffff;"
        "}"

        /* ===== STATUSBAR ===== */
        "#statusbar {"
        "    background-color: #f0f0f0;"
        "    color: #666666;"
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
        "    background-color: #f7f7f7;"
        "    color: #666666;"
        "    border-top: 1px solid #e6e6e6;"
        "    padding: 6px 10px;"
        "    font-size: 12px;"
        "}";

    gtk_css_provider_load_from_data(provider, css, -1, NULL);

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

    if (app->hun_en) {
        Hunspell_destroy(app->hun_en);
    }
    if (app->hun_fr) {
        Hunspell_destroy(app->hun_fr);
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