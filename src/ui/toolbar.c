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

    // Horizontal container
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    // Create "Correct" button
    btn_correct = gtk_button_new_with_label("Correct");
    g_signal_connect(btn_correct, "clicked", G_CALLBACK(on_correct_clicked), NULL);

    // Create "Rewrite" button
    btn_rewrite = gtk_button_new_with_label("Rewrite");
    g_signal_connect(btn_rewrite, "clicked", G_CALLBACK(on_rewrite_clicked), NULL);

    // Add buttons to toolbar
    gtk_box_pack_start(GTK_BOX(box), btn_correct, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(box), btn_rewrite, FALSE, FALSE, 5);

    return box;
}