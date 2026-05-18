#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <gtk/gtk.h>

/* =========================================================
   CALLBACKS DES BOUTONS ET MENUS EXISTANTS
   ========================================================= */
void on_correct_clicked(GtkWidget *widget, gpointer data);
void on_rewrite_clicked(GtkWidget *widget, gpointer data);
void on_llm_rewrite_clicked(GtkWidget *widget, gpointer data);
void on_llm_grammar_clicked(GtkWidget *widget, gpointer data);
void on_start_clicked(GtkWidget *widget, gpointer data);
void on_quit_clicked(GtkWidget *widget, gpointer data);

/* File menu */
void on_file_import_clicked(GtkWidget *widget, gpointer data);
void on_file_new_page_clicked(GtkWidget *widget, gpointer data);
void on_file_new_document_clicked(GtkWidget *widget, gpointer data);
void on_file_save_clicked(GtkWidget *widget, gpointer data);
void on_file_save_as_clicked(GtkWidget *widget, gpointer data);

/* Edit menu */
void on_edit_undo_clicked(GtkWidget *widget, gpointer data);
void on_edit_redo_clicked(GtkWidget *widget, gpointer data);
void on_edit_cut_clicked(GtkWidget *widget, gpointer data);
void on_edit_copy_clicked(GtkWidget *widget, gpointer data);
void on_edit_paste_clicked(GtkWidget *widget, gpointer data);
void on_edit_select_all_clicked(GtkWidget *widget, gpointer data);
void on_edit_find_clicked(GtkWidget *widget, gpointer data);

/* View menu */
void on_view_toggle_horizontal_ruler_clicked(GtkWidget *widget, gpointer data);
void on_view_toggle_vertical_ruler_clicked(GtkWidget *widget, gpointer data);

/* Insert menu */
void on_insert_date_clicked(GtkWidget *widget, gpointer data);
void on_insert_image_clicked(GtkWidget *widget, gpointer data);
void on_insert_table_clicked(GtkWidget *widget, gpointer data);
void on_insert_page_break_clicked(GtkWidget *widget, gpointer data);
void on_insert_header_clicked(GtkWidget *widget, gpointer data);
void on_insert_footer_clicked(GtkWidget *widget, gpointer data);
void on_insert_link_clicked(GtkWidget *widget, gpointer data);
void on_insert_comment_clicked(GtkWidget *widget, gpointer data);

/* Format menu & toolbar (Doublons nettoyés ici) */
void on_format_bold_clicked(GtkWidget *widget, gpointer data);
void on_format_italic_clicked(GtkWidget *widget, gpointer data);
void on_format_underline_clicked(GtkWidget *widget, gpointer data);
void on_format_font_size_changed(GtkComboBox *widget, gpointer data);
void on_format_font_family_changed(GtkComboBox *widget, gpointer data);
void on_format_align_left_clicked(GtkWidget *widget, gpointer data);
void on_format_align_center_clicked(GtkWidget *widget, gpointer data);
void on_format_align_right_clicked(GtkWidget *widget, gpointer data);

/* =========================================================
   GESTION DES ÉVÉNEMENTS DU TEXTBUFFER
   ========================================================= */
void on_text_inserted(GtkTextBuffer *textbuffer, GtkTextIter *location, 
                      const gchar *text, gint len, gpointer data);
void on_text_deleted(GtkTextBuffer *textbuffer, GtkTextIter *start, 
                     GtkTextIter *end, gpointer data);
void on_text_changed(GtkTextBuffer *textbuffer, gpointer data);
void on_text_modified(GtkTextBuffer *textbuffer, gpointer data);
void on_text_cursor_moved(GtkTextBuffer *textbuffer, GtkTextIter *location, gpointer data);

/* =========================================================
   NOUVEAUX CALLBACKS : TOOLS, TABLE, WINDOW ET HELP
   ========================================================= */

/* Tools menu */
void on_tools_word_count_clicked(GtkWidget *widget, gpointer data);
void on_tools_spellcheck_clicked(GtkWidget *widget, gpointer data);
void on_tools_preferences_clicked(GtkWidget *widget, gpointer data);

/* Table menu */
void on_table_insert_row_above_clicked(GtkWidget *widget, gpointer data);
void on_table_insert_row_below_clicked(GtkWidget *widget, gpointer data);
void on_table_insert_col_left_clicked(GtkWidget *widget, gpointer data);
void on_table_insert_col_right_clicked(GtkWidget *widget, gpointer data);
void on_table_delete_row_clicked(GtkWidget *widget, gpointer data);
void on_table_delete_col_clicked(GtkWidget *widget, gpointer data);

/* Window menu */
void on_window_new_clicked(GtkWidget *widget, gpointer data);
void on_window_fullscreen_clicked(GtkWidget *widget, gpointer data);

/* Help menu */
void on_help_contents_clicked(GtkWidget *widget, gpointer data);
void on_help_about_clicked(GtkWidget *widget, gpointer data);

#endif /* CALLBACKS_H */