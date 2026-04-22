#include "../../include/ui/window.h"
#include "../../include/ui/editor.h"
#include "../../include/ui/sidebar.h"
#include "../../include/ui/toolbar.h"
#include "../../include/ui/callbacks.h"

// ======================================
// MODULE: WINDOW
// ROLE: Create the main application window
// ======================================

static void setup_css(void);
static GtkWidget* create_welcome_page(AppWidgets *app_widgets);
static GtkWidget* create_editor_page(AppWidgets *app_widgets);
static Hunhandle* create_hunspell_dictionary(const gchar *lang);
static void cleanup_app_widgets(GtkWidget *widget, gpointer data);

void create_main_window(GtkApplication *app, gpointer user_data) {
    (void)user_data;

    GtkWidget *window;
    GtkWidget *stack;
    GtkWidget *welcome_page;
    GtkWidget *editor_page;
    AppWidgets *app_widgets;

    setup_css();

    // Allocate state shared between callbacks and pages
    app_widgets = g_new0(AppWidgets, 1);
    app_widgets->app = app;
    app_widgets->hun_en = create_hunspell_dictionary("en_US");
    app_widgets->hun_fr = create_hunspell_dictionary("fr_FR");

    // Create the main application window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "IntelliEditor");
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 650);
    gtk_container_set_border_width(GTK_CONTAINER(window), 16);
    gtk_widget_set_name(window, "app-window");

    // Create a stack to switch between the welcome screen and the editor
    stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(stack), 250);
    gtk_container_add(GTK_CONTAINER(window), stack);
    app_widgets->stack = stack;

    welcome_page = create_welcome_page(app_widgets);
    editor_page = create_editor_page(app_widgets);

    gtk_stack_add_titled(GTK_STACK(stack), welcome_page, "welcome", "Accueil");
    gtk_stack_add_titled(GTK_STACK(stack), editor_page, "editor", "Éditeur");
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "welcome");

    g_signal_connect(window, "destroy", G_CALLBACK(cleanup_app_widgets), app_widgets);
    g_signal_connect(window, "destroy", G_CALLBACK(g_application_quit), app);
    gtk_widget_show_all(window);
}

static Hunhandle* create_hunspell_dictionary(const gchar *lang) {
    gchar *aff_path;
    gchar *dic_path;
    Hunhandle *handle = NULL;

    aff_path = g_strdup_printf("/usr/share/hunspell/%s.aff", lang);
    dic_path = g_strdup_printf("/usr/share/hunspell/%s.dic", lang);

    if (g_file_test(aff_path, G_FILE_TEST_IS_REGULAR) && g_file_test(dic_path, G_FILE_TEST_IS_REGULAR)) {
        handle = Hunspell_create(aff_path, dic_path);
        if (!handle) {
            g_warning("Hunspell: impossible de charger %s / %s", aff_path, dic_path);
        }
    } else {
        g_warning("Hunspell: dictionnaire introuvable %s / %s", aff_path, dic_path);
    }

    g_free(aff_path);
    g_free(dic_path);
    return handle;
}

static GtkWidget* create_welcome_page(AppWidgets *app_widgets) {
    GtkWidget *box;
    GtkWidget *title;
    GtkWidget *subtitle;
    GtkWidget *button_box;
    GtkWidget *start_button;
    GtkWidget *quit_button;

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
    gtk_widget_set_name(box, "welcome-page");
    gtk_container_set_border_width(GTK_CONTAINER(box), 40);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

    title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title),
        "<span size='xx-large' weight='bold'>Bienvenue dans IntelliEditor</span>");
    gtk_label_set_justify(GTK_LABEL(title), GTK_JUSTIFY_CENTER);
    gtk_widget_set_name(title, "welcome-title");

    subtitle = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(subtitle),
        "<span size='medium'>Un éditeur intelligent pour corriger, réécrire et améliorer votre texte.</span>");
    gtk_label_set_justify(GTK_LABEL(subtitle), GTK_JUSTIFY_CENTER);
    gtk_label_set_line_wrap(GTK_LABEL(subtitle), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(subtitle), 45);
    gtk_widget_set_name(subtitle, "welcome-subtitle");

    button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
    gtk_widget_set_halign(button_box, GTK_ALIGN_CENTER);

    start_button = gtk_button_new_with_label("Ouvrir l'éditeur");
    gtk_widget_set_name(start_button, "start-button");
    g_signal_connect(start_button, "clicked", G_CALLBACK(on_start_clicked), app_widgets->stack);

    quit_button = gtk_button_new_with_label("Quitter");
    gtk_widget_set_name(quit_button, "quit-button");
    g_signal_connect(quit_button, "clicked", G_CALLBACK(on_quit_clicked), NULL);

    gtk_box_pack_start(GTK_BOX(button_box), start_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(button_box), quit_button, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(box), title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), subtitle, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), button_box, FALSE, FALSE, 0);

    return box;
}

static GtkWidget* create_editor_page(AppWidgets *app_widgets) {
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *toolbar;
    GtkWidget *editor;
    GtkWidget *sidebar;
    GtkWidget *heading;

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_name(vbox, "editor-page");
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 12);

    heading = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(heading), "<span size='x-large' weight='bold'>IntelliEditor</span>");
    gtk_widget_set_halign(heading, GTK_ALIGN_START);
    gtk_widget_set_name(heading, "editor-heading");

    toolbar = create_toolbar(app_widgets);
    editor = create_editor();
    app_widgets->editor_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor));
    sidebar = create_sidebar();
    app_widgets->sidebar = sidebar;

    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 14);
    gtk_box_pack_start(GTK_BOX(hbox), editor, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), sidebar, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(vbox), heading, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

    return vbox;
}

static void cleanup_app_widgets(GtkWidget *widget, gpointer data) {
    AppWidgets *app_widgets = (AppWidgets *)data;
    (void)widget;

    if (app_widgets->hun_en) {
        Hunspell_destroy(app_widgets->hun_en);
    }
    if (app_widgets->hun_fr) {
        Hunspell_destroy(app_widgets->hun_fr);
    }
    g_free(app_widgets);
}

static void setup_css(void) {
    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;
    const gchar *css =
        "window#app-window { background-color: #282c34; color: #edf2f7; }"
        "#welcome-page { background-color: #2b2f3d; border-radius: 18px; padding: 24px; }"
        "#welcome-title { color: #f8f8f2; margin-bottom: 12px; }"
        "#welcome-subtitle { color: #d7dae0; }"
        "#start-button, #quit-button, #correct-button, #rewrite-button {"
        "  min-width: 160px; padding: 12px 18px; border-radius: 8px; font-weight: bold; }"
        "#start-button { background-image: linear-gradient(#61afef, #528bcc); color: #ffffff; }"
        "#quit-button { background-image: linear-gradient(#e06c75, #c65a63); color: #ffffff; }"
        "#correct-button, #rewrite-button { background-image: linear-gradient(#98c379, #83b36a); color: #222222; }"
        "#editor-page { background-color: transparent; }"
        "#editor-heading { color: #f8f8f2; margin-bottom: 8px; }"
        ".sidebar { background-color: #21252b; border-radius: 10px; padding: 14px; }"
        ".sidebar label { color: #abb2bf; }"
        "GtkSourceView { background-color: #1e222a; color: #e5c07b; }"
        "GtkButton { border-width: 0; }";

    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen,
        GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);
}
