#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <gtk/gtk.h>

/* Callbacks des boutons */
void on_correct_clicked(GtkWidget *widget, gpointer data);
void on_rewrite_clicked(GtkWidget *widget, gpointer data);
void on_start_clicked(GtkWidget *widget, gpointer data);
void on_quit_clicked(GtkWidget *widget, gpointer data);
void on_file_import_clicked(GtkWidget *widget, gpointer data);
void on_file_save_clicked(GtkWidget *widget, gpointer data);
void on_text_inserted(GtkTextBuffer *textbuffer, GtkTextIter *location, 
                      const gchar *text, gint len, gpointer data);

void on_text_deleted(GtkTextBuffer *textbuffer, GtkTextIter *start, 
                     GtkTextIter *end, gpointer data);

#endif
