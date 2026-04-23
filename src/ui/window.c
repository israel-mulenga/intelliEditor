#include "../../include/ui/window.h"
#include "../../include/ui/editor.h"
#include "../../include/ui/sidebar.h"
#include "../../include/ui/toolbar.h"
#include "../../include/ui/callbacks.h"
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
    (void)user_data; // inutilisé

    GtkWidget *window;
    GtkWidget *stack;

    /* Allocation structure globale */
    AppWidgets *app_widgets = g_new0(AppWidgets, 1);

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
   PAGE ÉDITEUR
   ========================================================= */
GtkWidget* create_editor_page(AppWidgets *app_widgets) {

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);

    /* ================= TOOLBAR ================= */
    GtkWidget *toolbar = create_toolbar(app_widgets);

    /* ================= EDITOR ================= */
    GtkWidget *editor = create_editor();

    // On récupère le buffer depuis le widget et on le stocke dans la structure
    app_widgets->editor_buffer = GTK_SOURCE_BUFFER(
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor))
    );
    GtkWidget *sidebar = create_sidebar();
    app_widgets->sidebar = sidebar;

    /* ================= STATUSBAR ================= */
    GtkWidget *statusbar = gtk_statusbar_new();
    app_widgets->statusbar = statusbar;

    /* ================= LAYOUT ================= */

    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);

    gtk_paned_pack1(GTK_PANED(paned), editor, TRUE, FALSE);
    gtk_paned_pack2(GTK_PANED(paned), sidebar, FALSE, FALSE);

    gtk_box_pack_start(GTK_BOX(vbox), paned, TRUE, TRUE, 0);

    gtk_widget_set_name(statusbar, "statusbar");
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
        "    background-color: #ffffff;"
        "    color: #1e1e1e;"
        "    font-family: 'Inter', 'Segoe UI', sans-serif;"
        "}"

        /* ===== TOOLBAR SIMPLE ===== */
        "#toolbar {"
        "    background-color: #f7f7f7;"
        "    border-bottom: 1px solid #e6e6e6;"
        "    padding: 10px;"
        "}"

        /* ===== ZONE D'ÉDITION (IMPORTANT) ===== */
        "#editor-textview {"
        "    background-color: #ffffff;"
        "    color: #1e1e1e;"
        "    padding: 20px;"
        "    border: none;"
        "    font-size: 14px;"
        "}"

        "textview text {"
        "    background-color: #ffffff;"
        "    color: #1e1e1e;"
        "    selection-background-color: #cce5ff;"
        "    selection-color: #000000;"
        "}"

        /* ===== SIDEBAR SOBRE ===== */
        "#sidebar {"
        "    background-color: #f9f9f9;"
        "    border-left: 1px solid #eaeaea;"
        "    padding: 12px;"
        "}"

        /* ===== BOUTONS SIMPLES (STYLE ÉDITEUR) ===== */
        "button {"
        "    background-color: #ffffff;"
        "    color: #1e1e1e;"
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