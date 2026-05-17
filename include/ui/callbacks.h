#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <gtk/gtk.h>

/* Callbacks des boutons */
void on_correct_clicked(GtkWidget *widget, gpointer data);
void on_rewrite_clicked(GtkWidget *widget, gpointer data);
void on_llm_rewrite_clicked(GtkWidget *widget, gpointer data);
void on_llm_grammar_clicked(GtkWidget *widget, gpointer data);
void on_start_clicked(GtkWidget *widget, gpointer data);
void on_quit_clicked(GtkWidget *widget, gpointer data);
void on_file_import_clicked(GtkWidget *widget, gpointer data);
void on_file_new_page_clicked(GtkWidget *widget, gpointer data);
void on_file_new_document_clicked(GtkWidget *widget, gpointer data);
void on_file_save_clicked(GtkWidget *widget, gpointer data);
void on_file_save_as_clicked(GtkWidget *widget, gpointer data);
void on_edit_undo_clicked(GtkWidget *widget, gpointer data); // Callback pour le bouton Undo dans le menu Edit
void on_edit_redo_clicked(GtkWidget *widget, gpointer data); // Callback pour le bouton Redo dans le menu Edit
void on_edit_cut_clicked(GtkWidget *widget, gpointer data); // Callback pour Cut
void on_view_toggle_horizontal_ruler_clicked(GtkWidget *widget, gpointer data);
void on_view_toggle_vertical_ruler_clicked(GtkWidget *widget, gpointer data);
void on_edit_copy_clicked(GtkWidget *widget, gpointer data); // Callback pour Copy
void on_edit_paste_clicked(GtkWidget *widget, gpointer data); // Callback pour Paste
void on_edit_select_all_clicked(GtkWidget *widget, gpointer data); // Callback pour Select All
void on_edit_find_clicked(GtkWidget *widget, gpointer data); // Callback pour Find
void on_text_inserted(GtkTextBuffer *textbuffer, GtkTextIter *location, 
                      const gchar *text, gint len, gpointer data);

void on_text_deleted(GtkTextBuffer *textbuffer, GtkTextIter *start, 
                     GtkTextIter *end, gpointer data);

void on_text_changed(GtkTextBuffer *textbuffer, gpointer data);

void on_text_modified(GtkTextBuffer *textbuffer, gpointer data);

void on_text_cursor_moved(GtkTextBuffer *textbuffer, GtkTextIter *location, gpointer data);

void on_insert_date_clicked(GtkWidget *widget, gpointer data);
void on_insert_image_clicked(GtkWidget *widget, gpointer data);
void on_insert_table_clicked(GtkWidget *widget, gpointer data);
void on_insert_page_break_clicked(GtkWidget *widget, gpointer data);
void on_insert_header_clicked(GtkWidget *widget, gpointer data);
void on_insert_footer_clicked(GtkWidget *widget, gpointer data);
void on_insert_link_clicked(GtkWidget *widget, gpointer data);
void on_insert_comment_clicked(GtkWidget *widget, gpointer data);

#endif
