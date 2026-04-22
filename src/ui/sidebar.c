#include <gtk/gtk.h>
#include "ui/editor.h"

// ======================================
// MODULE: SIDEBAR
// ROLE: Display rules and analysis results
// ======================================

GtkWidget* create_sidebar() {

    GtkWidget *box;
    GtkWidget *label;

    // Create vertical container for the sidebar panel
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_name(box, "sidebar");
    gtk_container_set_border_width(GTK_CONTAINER(box), 10);

    // Temporary text (will be replaced later)
    label = gtk_label_new("Mots incorrects s'afficheront ici.");
    gtk_widget_set_name(label, "sidebar-label");
    g_object_set_data(G_OBJECT(box), "sidebar-label", label);

    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 5);

    return box;
}
