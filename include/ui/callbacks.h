#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <gtk/gtk.h>

/* Callbacks des boutons */
void on_correct_clicked(GtkWidget *widget, gpointer data);
void on_rewrite_clicked(GtkWidget *widget, gpointer data);
void on_start_clicked(GtkWidget *widget, gpointer data);
void on_quit_clicked(GtkWidget *widget, gpointer data);

#endif
