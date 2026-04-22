#include "ui/toolbar.h"
#include "ui/callbacks.h"

// ======================================
// MODULE: TOOLBAR
// ROLE: Create action buttons
// ======================================

GtkWidget* create_toolbar() {

    GtkWidget *box;
    GtkWidget *btn_correct;
    GtkWidget *btn_rewrite;

    // Horizontal container for the toolbar buttons
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_name(box, "toolbar-box");

    // Create "Correct" button
    btn_correct = gtk_button_new_with_label("Correct");
    gtk_widget_set_name(btn_correct, "correct-button");
    g_signal_connect(btn_correct, "clicked", G_CALLBACK(on_correct_clicked), NULL);

    // Create "Rewrite" button
    btn_rewrite = gtk_button_new_with_label("Rewrite");
    gtk_widget_set_name(btn_rewrite, "rewrite-button");
    g_signal_connect(btn_rewrite, "clicked", G_CALLBACK(on_rewrite_clicked), NULL);

    // Add buttons to toolbar
    gtk_box_pack_start(GTK_BOX(box), btn_correct, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), btn_rewrite, FALSE, FALSE, 0);

    return box;
}
