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