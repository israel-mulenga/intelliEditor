#include "../../include/ui/window.h"
#include "../../include/ui/editor.h"
#include "../../include/ui/sidebar.h"
#include "../../include/ui/toolbar.h"

// ======================================
// MODULE: WINDOW
// ROLE: Create the main application window
// ======================================

void create_main_window(GtkApplication *app, gpointer user_data) {

    GtkWidget *window;
    GtkWidget *vbox;        // vertical container
    GtkWidget *hbox;        // horizontal container
    GtkWidget *editor;
    GtkWidget *sidebar;
    GtkWidget *toolbar;

    // Create the main window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "IntelliEditor");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 600);

    // Create a vertical box (top -> bottom)
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create toolbar (top part)
    toolbar = create_toolbar();
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);

    // Create horizontal box (editor + sidebar)
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

    // Create editor (text area)
    editor = create_editor();

    // Create sidebar (rules panel)
    sidebar = create_sidebar();

    // Add editor (left side)
    gtk_box_pack_start(GTK_BOX(hbox), editor, TRUE, TRUE, 5);

    // Add sidebar (right side)
    gtk_box_pack_start(GTK_BOX(hbox), sidebar, FALSE, FALSE, 5);

    // Show everything
    gtk_widget_show_all(window);
}