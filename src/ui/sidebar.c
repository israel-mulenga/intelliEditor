#include "ui/editor.h"

// ======================================
// MODULE: SIDEBAR
// ROLE: Display rules and analysis results
// ======================================

GtkWidget* create_sidebar() {

    GtkWidget *box;
    GtkWidget *label;

    // Create vertical container
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    // Temporary text (will be replaced later)
    label = gtk_label_new("Rules will appear here");

    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 5);

    return box;
}