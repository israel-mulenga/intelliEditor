#include "ui/callbacks.h"
#include <stdio.h>

// ======================================
// MODULE: CALLBACKS
// ROLE: Handle user actions (button clicks)
// ======================================

// Called when "Correct" button is clicked
void on_correct_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    (void)data;
    printf("Correct button clicked\n");
}

// Called when "Rewrite" button is clicked
void on_rewrite_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    (void)data;
    printf("Rewrite button clicked\n");
}

// Called when the welcome screen "Ouvrir l'éditeur" button is clicked
void on_start_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    GtkStack *stack = GTK_STACK(data);
    gtk_stack_set_visible_child_name(stack, "editor");
}

// Called when the "Quitter" button is clicked
void on_quit_clicked(GtkWidget *widget, gpointer data) {
    (void)data;
    GtkWidget *window = gtk_widget_get_toplevel(widget);
    gtk_widget_destroy(window);
}
