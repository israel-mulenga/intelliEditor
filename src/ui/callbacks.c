#include "ui/callbacks.h"
#include "ui/window.h"
#include "editor/file_manager.h"
#include "llm/llm_bridge.h"
#include "nlp/nlp_highlighter.h"
#include <stdio.h>
#include <stdlib.h>

// ======================================
// MODULE: CALLBACKS
// ROLE: Handle user actions (button clicks)
// ======================================

static gboolean is_word_correct(AppWidgets *app_widgets, const gchar *word) {
    if (app_widgets->hun_en && Hunspell_spell(app_widgets->hun_en, word)) {
        return TRUE;
    }
    if (app_widgets->hun_fr && Hunspell_spell(app_widgets->hun_fr, word)) {
        return TRUE;
    }
    return FALSE;
}

static gchar* check_spelling(AppWidgets *app_widgets) {
    GtkTextIter start, end;
    gchar *text;
    gchar **words;
    GString *result;
    gint wrong_count = 0;
    const gchar *separators = " \t\n\r.,;:!?\"'()[]{}<>@#$%^&*-_=+/\\|`~";

    if (!app_widgets->editor_buffer) {
        return g_strdup("Erreur : éditeur non disponible.");
    }

    gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(app_widgets->editor_buffer), &start);
    gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(app_widgets->editor_buffer), &end);
    text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(app_widgets->editor_buffer), &start, &end, FALSE);
    words = g_strsplit_set(text, separators, -1);
    result = g_string_new(NULL);

    if (!app_widgets->hun_en && !app_widgets->hun_fr) {
        g_string_assign(result, "Dictionnaires Hunspell non trouvés. Installez hunspell et les dictionnaires.");
    } else {
        for (guint i = 0; words[i] != NULL; i++) {
            gchar *word = words[i];
            gchar *clean;
            gchar *lower;

            clean = g_strstrip(word);
            if (*clean == '\0') {
                continue;
            }

            lower = g_utf8_strdown(clean, -1);
            if (!is_word_correct(app_widgets, lower)) {
                if (wrong_count == 0) {
                    g_string_append(result, "Mots incorrects :\n");
                }
                g_string_append_printf(result, "%d. %s\n", wrong_count + 1, lower);
                wrong_count++;
            }
            g_free(lower);
        }

        if (wrong_count == 0) {
            g_string_assign(result, "Aucun mot incorrect trouvé.");
        }
    }

    g_strfreev(words);
    g_free(text);
    return g_string_free(result, FALSE);
}

static void update_sidebar_text(AppWidgets *app_widgets, const gchar *text) {
    if (!app_widgets || !app_widgets->sidebar) {
        return;
    }

    GtkWidget *label = GTK_WIDGET(g_object_get_data(G_OBJECT(app_widgets->sidebar), "sidebar-label"));
    if (GTK_IS_LABEL(label)) {
        gtk_label_set_text(GTK_LABEL(label), text);
    }
}

static void sync_editor_from_gap_buffer(AppWidgets *app_widgets) {
    if (!app_widgets || !app_widgets->editor_buffer || !app_widgets->gb) {
        return;
    }

    gchar *content = gap_buffer_get_content(app_widgets->gb);
    if (!content) {
        return;
    }

    if (app_widgets->insert_handler_id != 0) {
        g_signal_handler_block(app_widgets->editor_buffer, app_widgets->insert_handler_id);
    }
    if (app_widgets->delete_handler_id != 0) {
        g_signal_handler_block(app_widgets->editor_buffer, app_widgets->delete_handler_id);
    }

    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(app_widgets->editor_buffer), content, -1);

    if (app_widgets->delete_handler_id != 0) {
        g_signal_handler_unblock(app_widgets->editor_buffer, app_widgets->delete_handler_id);
    }
    if (app_widgets->insert_handler_id != 0) {
        g_signal_handler_unblock(app_widgets->editor_buffer, app_widgets->insert_handler_id);
    }

    g_free(content);
}

static gchar *prompt_save_filename(AppWidgets *app) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Save Text File",
        GTK_WINDOW(app->window),
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Save", GTK_RESPONSE_ACCEPT,
        NULL
    );

    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);

    if (app->current_file_path) {
        gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), app->current_file_path);
    }

    gchar *filename = NULL;
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    }

    gtk_widget_destroy(dialog);
    return filename;
}

static void save_to_path(AppWidgets *app, const gchar *filename, gboolean update_current_path) {
    if (!filename) {
        return;
    }

    if (!gap_buffer_save_to_file(app->gb, filename)) {
        g_printerr("Unable to save file: %s\n", filename);
        return;
    }

    if (update_current_path) {
        g_free(app->current_file_path);
        app->current_file_path = g_strdup(filename);
    }

    app_autosave_reschedule(app);
}

// Called when "Correct" button is clicked
void on_correct_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app_widgets = (AppWidgets *)data;
    gchar *message = check_spelling(app_widgets);
    update_sidebar_text(app_widgets, message);
    g_free(message);
}

// Called when "Rewrite" button is clicked
void on_rewrite_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    (void)data;
    printf("Rewrite button clicked\n");
}

// Wrapper to call LLM bridge rephrase using current app widgets
void setup_nlp_callbacks(GtkWidget *btn_rewrite,
                         GtkWidget *btn_grammar,
                         AppWidgets *app_widgets)
{
    g_signal_connect(
        btn_rewrite,
        "clicked",
        G_CALLBACK(on_rewrite_clicked),
        app_widgets
    );

    g_signal_connect(
        btn_grammar,
        "clicked",
        G_CALLBACK(on_grammar_clicked),
        app_widgets
    );
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

void on_file_import_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;

    if (!app || !app->window) {
        return;
    }

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Import Text File",
        GTK_WINDOW(app->window),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Import", GTK_RESPONSE_ACCEPT,
        NULL
    );

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Text files");
    gtk_file_filter_add_mime_type(filter, "text/plain");
    gtk_file_filter_add_pattern(filter, "*.txt");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        GapBuffer *loaded = gap_buffer_load_from_file(filename);

        if (loaded) {
            if (app->gb) {
                gap_buffer_destroy(app->gb);
            }
            app->gb = loaded;

            sync_editor_from_gap_buffer(app);

            g_free(app->current_file_path);
            app->current_file_path = g_strdup(filename);
            app_autosave_reschedule(app);
        } else {
            g_printerr("Unable to import file: %s\n", filename);
        }

        g_free(filename);
    }

    gtk_widget_destroy(dialog);

    if (app->editor_view) {
        gtk_widget_grab_focus(app->editor_view);
    }
}

void on_file_save_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;

    if (!app || !app->gb || !app->window) {
        return;
    }

    if (app->current_file_path) {
        save_to_path(app, app->current_file_path, FALSE);
    } else {
        gchar *filename = prompt_save_filename(app);
        save_to_path(app, filename, filename != NULL);
        g_free(filename);
    }
}

void on_file_save_as_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;

    if (!app || !app->gb || !app->window) {
        return;
    }

    gchar *filename = prompt_save_filename(app);
    save_to_path(app, filename, filename != NULL);
    g_free(filename);
}

void on_text_inserted(GtkTextBuffer *textbuffer, GtkTextIter *location,
                      const gchar *text, gint len, gpointer data) {
    (void)textbuffer;
    AppWidgets *app = (AppWidgets *)data;

    if (!app || !app->gb || !location || !text || len <= 0) {
        return;
    }

    gint offset = gtk_text_iter_get_offset(location);
    gap_buffer_move_cursor(app->gb, (size_t)offset);

    for (gint i = 0; i < len; i++) {
        gap_buffer_insert(app->gb, text[i]);
    }
}

void on_text_deleted(GtkTextBuffer *textbuffer, GtkTextIter *start, 
                     GtkTextIter *end, gpointer data) {
    (void)textbuffer;
    AppWidgets *app = (AppWidgets *)data;

    if (!app || !app->gb || !start || !end) {
        return;
    }

    gint start_offset = gtk_text_iter_get_offset(start);
    gint end_offset = gtk_text_iter_get_offset(end);
    gint len = end_offset - start_offset;

    if (len <= 0) {
        return;
    }

    gap_buffer_move_cursor(app->gb, (size_t)start_offset);

    for (gint i = 0; i < len; i++) {
        gap_buffer_delete(app->gb);
    }
}

void on_edit_undo_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    
    g_signal_handler_disconnect(app->editor_buffer, app->insert_handler_id);
    g_signal_handler_disconnect(app->editor_buffer, app->delete_handler_id);
    
    gap_buffer_undo(app->gb);
    
    gchar *content = gap_buffer_get_content(app->gb);
    if (content) {
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(app->editor_buffer), content, -1);
        g_free(content); // Correction ici : g_free au lieu de free
    }
    
    app->insert_handler_id = g_signal_connect(app->editor_buffer, "insert-text",
                    G_CALLBACK(on_text_inserted), app);
    app->delete_handler_id = g_signal_connect(app->editor_buffer, "delete-range",
                    G_CALLBACK(on_text_deleted), app);
}

void on_edit_redo_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    
    g_signal_handler_disconnect(app->editor_buffer, app->insert_handler_id);
    g_signal_handler_disconnect(app->editor_buffer, app->delete_handler_id);
    
    gap_buffer_redo(app->gb);
    
    gchar *content = gap_buffer_get_content(app->gb);
    if (content) {
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(app->editor_buffer), content, -1);
        g_free(content); // Correction ici : g_free au lieu de free
    }
    
    app->insert_handler_id = g_signal_connect(app->editor_buffer, "insert-text",
                    G_CALLBACK(on_text_inserted), app);
    app->delete_handler_id = g_signal_connect(app->editor_buffer, "delete-range",
                    G_CALLBACK(on_text_deleted), app);
}

void on_edit_select_all_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    if (app->editor_buffer) {
        GtkTextIter start, end;
        gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(app->editor_buffer), &start);
        gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(app->editor_buffer), &end);
        gtk_text_buffer_select_range(GTK_TEXT_BUFFER(app->editor_buffer), &start, &end);
    }
}

void on_edit_cut_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    if (app->editor_buffer) {
        gtk_text_buffer_cut_clipboard(GTK_TEXT_BUFFER(app->editor_buffer), gtk_clipboard_get(GDK_SELECTION_CLIPBOARD), TRUE);
    }
}

void on_edit_copy_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    if (app->editor_buffer) {
        gtk_text_buffer_copy_clipboard(GTK_TEXT_BUFFER(app->editor_buffer), gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
    }
}

void on_edit_paste_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    if (app->editor_buffer) {
        gtk_text_buffer_paste_clipboard(GTK_TEXT_BUFFER(app->editor_buffer), gtk_clipboard_get(GDK_SELECTION_CLIPBOARD), NULL, TRUE);
    }
}

void on_edit_find_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(app->window),
        GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
        "Find functionality not yet implemented.");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void on_view_toggle_horizontal_ruler_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *app = (AppWidgets *)data;
    gboolean active = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget));
    if (!app || !app->horizontal_ruler) {
        return;
    }
    if (active) {
        gtk_widget_show(app->horizontal_ruler);
    } else {
        gtk_widget_hide(app->horizontal_ruler);
    }
}

void on_view_toggle_vertical_ruler_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *app = (AppWidgets *)data;
    gboolean active = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget));
    if (!app || !app->vertical_ruler) {
        return;
    }
    if (active) {
        gtk_widget_show(app->vertical_ruler);
    } else {
        gtk_widget_hide(app->vertical_ruler);
    }
}

void on_file_new_page_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app_widgets = (AppWidgets *)data;
    append_new_page_to_current_document(app_widgets);
}

void on_file_new_document_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app_widgets = (AppWidgets *)data;
    static int doc_count = 2;
    gchar title[64];
    sprintf(title, "Document %d", doc_count++);
    create_new_tab(app_widgets, title);
}

/* =========================================================
   INSERT ELEMENTS CALLBACKS
   ========================================================= */
void on_insert_date_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app_widgets = (AppWidgets *)data;
    GtkTextIter iter;

    gtk_text_buffer_get_iter_at_mark(
        GTK_TEXT_BUFFER(app_widgets->editor_buffer),
        &iter,
        gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(app_widgets->editor_buffer))
    );

    gtk_text_buffer_insert(GTK_TEXT_BUFFER(app_widgets->editor_buffer), &iter, "16/05/2026 14:35", -1);
}

void on_insert_image_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app_widgets = (AppWidgets *)data;
    GtkTextIter iter;

    gtk_text_buffer_get_iter_at_mark(
        GTK_TEXT_BUFFER(app_widgets->editor_buffer),
        &iter,
        gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(app_widgets->editor_buffer))
    );

    // Insert image placeholder (les icônes symboliques retournent NULL)
    gtk_text_buffer_insert(GTK_TEXT_BUFFER(app_widgets->editor_buffer), &iter, "[Image: <chemin-vers-image>]\n", -1);
}

void on_insert_table_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    if (app->editor_buffer) {
        GtkTextIter iter;
        gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(app->editor_buffer), &iter, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(app->editor_buffer)));
        gtk_text_buffer_insert(GTK_TEXT_BUFFER(app->editor_buffer), &iter, "Table\n", -1);
    }
}

void on_insert_page_break_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    if (app->editor_buffer) {
        GtkTextIter iter;
        gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(app->editor_buffer), &iter, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(app->editor_buffer)));
        gtk_text_buffer_insert(GTK_TEXT_BUFFER(app->editor_buffer), &iter, "\f", -1);
    }
}

void on_insert_header_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    if (app->editor_buffer) {
        GtkTextIter iter;
        gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(app->editor_buffer), &iter, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(app->editor_buffer)));
        gtk_text_buffer_insert(GTK_TEXT_BUFFER(app->editor_buffer), &iter, "Header\n", -1);
    }
}

void on_insert_footer_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    if (app->editor_buffer) {
        GtkTextIter iter;
        gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(app->editor_buffer), &iter, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(app->editor_buffer)));
        gtk_text_buffer_insert(GTK_TEXT_BUFFER(app->editor_buffer), &iter, "Footer\n", -1);
    }
}

void on_insert_link_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    if (app->editor_buffer) {
        GtkTextIter iter;
        gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(app->editor_buffer), &iter, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(app->editor_buffer)));
        gtk_text_buffer_insert(GTK_TEXT_BUFFER(app->editor_buffer), &iter, "https://www.example.com\n", -1);
    }
}

void on_insert_comment_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    if (app->editor_buffer) {
        GtkTextIter iter;
        gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(app->editor_buffer), &iter, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(app->editor_buffer)));
        gtk_text_buffer_insert(GTK_TEXT_BUFFER(app->editor_buffer), &iter, "[Commentaire]\n", -1);
    }
}

/* =========================================================
   FORMAT TOOLBAR CALLBACKS
   ========================================================= */

void on_format_bold_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    if (!app || !app->editor_buffer) return;

    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(app->editor_buffer);
    GtkTextIter start, end;

    if (gtk_text_buffer_get_selection_bounds(buffer, &start, &end)) {
        gtk_text_buffer_apply_tag_by_name(buffer, "bold", &start, &end);
        printf("[Format] Bold applied\n");
    }
}

void on_format_italic_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    if (!app || !app->editor_buffer) return;

    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(app->editor_buffer);
    GtkTextIter start, end;

    if (gtk_text_buffer_get_selection_bounds(buffer, &start, &end)) {
        gtk_text_buffer_apply_tag_by_name(buffer, "italic", &start, &end);
        printf("[Format] Italic applied\n");
    }
}

void on_format_underline_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    if (!app || !app->editor_buffer) return;

    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(app->editor_buffer);
    GtkTextIter start, end;

    if (gtk_text_buffer_get_selection_bounds(buffer, &start, &end)) {
        gtk_text_buffer_apply_tag_by_name(buffer, "underline", &start, &end);
        printf("[Format] Underline applied\n");
    }
}

void on_format_font_size_changed(GtkComboBox *widget, gpointer data) {
    AppWidgets *app = (AppWidgets *)data;
    if (!app || !app->editor_buffer) return;

    GtkComboBoxText *combo_box = GTK_COMBO_BOX_TEXT(widget);
    gchar *font_size_str = gtk_combo_box_text_get_active_text(combo_box);
    if (font_size_str == NULL) return;

    printf("[Format] Font size changed to: %s\n", font_size_str);

    int size_points = atoi(font_size_str);
    if (size_points > 0) {
        GtkTextBuffer *buffer = GTK_TEXT_BUFFER(app->editor_buffer);
        GtkTextIter start, end;

        if (gtk_text_buffer_get_selection_bounds(buffer, &start, &end)) {
            GtkTextTag *size_tag = gtk_text_buffer_create_tag(buffer, NULL, 
                                                              "size", size_points * PANGO_SCALE, 
                                                              NULL);
            gtk_text_buffer_apply_tag(buffer, size_tag, &start, &end);
        }
    }
    g_free(font_size_str);
}

void on_format_font_family_changed(GtkComboBox *widget, gpointer data) {
    AppWidgets *app = (AppWidgets *)data;
    if (!app || !app->editor_buffer) return;

    GtkComboBoxText *combo_box = GTK_COMBO_BOX_TEXT(widget);
    gchar *font_family = gtk_combo_box_text_get_active_text(combo_box);
    if (font_family == NULL) return;

    printf("[Format] Font family changed to: %s\n", font_family);

    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(app->editor_buffer);
    GtkTextIter start, end;

    if (gtk_text_buffer_get_selection_bounds(buffer, &start, &end)) {
        GtkTextTag *font_tag = gtk_text_buffer_create_tag(buffer, NULL, 
                                                          "family", font_family, 
                                                          NULL);
        gtk_text_buffer_apply_tag(buffer, font_tag, &start, &end);
    }
    g_free(font_family);
}

void on_format_align_left_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    if (!app || !app->editor_view) return;

    gtk_text_view_set_justification(GTK_TEXT_VIEW(app->editor_view), GTK_JUSTIFY_LEFT);
    printf("[Format] Align left applied\n");
}

void on_format_align_center_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    if (!app || !app->editor_view) return;

    gtk_text_view_set_justification(GTK_TEXT_VIEW(app->editor_view), GTK_JUSTIFY_CENTER);
    printf("[Format] Align center applied\n");
}

void on_format_align_right_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    if (!app || !app->editor_view) return;

    gtk_text_view_set_justification(GTK_TEXT_VIEW(app->editor_view), GTK_JUSTIFY_RIGHT);
    printf("[Format] Align right applied\n");
}

void on_format_align_justify_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    if (!app || !app->editor_view) return;

    gtk_text_view_set_justification(GTK_TEXT_VIEW(app->editor_view), GTK_JUSTIFY_FILL);
    printf("[Format] Align justify applied\n");
}

/* =========================================================
   NEW MODULES: TOOLS, TABLE, WINDOW & HELP
   ========================================================= */

/* Tools menu */
void on_tools_word_count_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    if (!app || !app->editor_buffer || !app->window) return;

    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(app->editor_buffer), &start, &end);
    gchar *text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(app->editor_buffer), &start, &end, FALSE);
    
    gint char_count = g_utf8_strlen(text, -1);
    gint word_count = 0;
    gboolean in_word = FALSE;
    
    for (gchar *p = text; *p; p = g_utf8_next_char(p)) {
        gunichar c = g_utf8_get_char(p);
        if (g_unichar_isspace(c)) {
            in_word = FALSE;
        } else if (!in_word) {
            word_count++;
            in_word = TRUE;
        }
    }
    
    gchar message[256];
    g_snprintf(message, sizeof(message), "Words: %d\nCharacters: %d", word_count, char_count);
    update_sidebar_text(app, message);
    
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(app->window),
        GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
        "Word Count");
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    
    g_free(text);
}

void on_tools_spellcheck_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    if (!app || !app->window) return;
    
    gchar *result = check_spelling(app);
    update_sidebar_text(app, result);
    
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(app->window),
        GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
        "Spellcheck Result");
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), "%s", result);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    
    g_free(result);
}

void on_tools_preferences_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    if (!app || !app->window) return;
    
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Preferences",
        GTK_WINDOW(app->window), GTK_DIALOG_DESTROY_WITH_PARENT,
        "_OK", GTK_RESPONSE_OK,
        "_Cancel", GTK_RESPONSE_CANCEL, NULL);
    
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 10);
    gtk_container_add(GTK_CONTAINER(content), grid);
    
    GtkWidget *label1 = gtk_label_new("Font Family:");
    gtk_grid_attach(GTK_GRID(grid), label1, 0, 0, 1, 1);
    GtkWidget *font_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(font_combo), "Calibri");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(font_combo), "Arial");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(font_combo), "Times New Roman");
    gtk_combo_box_set_active(GTK_COMBO_BOX(font_combo), 0);
    gtk_grid_attach(GTK_GRID(grid), font_combo, 1, 0, 1, 1);
    
    GtkWidget *label2 = gtk_label_new("Auto-save enabled:");
    gtk_grid_attach(GTK_GRID(grid), label2, 0, 1, 1, 1);
    GtkWidget *autosave_check = gtk_check_button_new();
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(autosave_check), TRUE);
    gtk_grid_attach(GTK_GRID(grid), autosave_check, 1, 1, 1, 1);
    
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

/* Table menu */
void on_table_insert_row_above_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    if (!app || !app->editor_buffer) return;
    
    GtkTextIter iter;
    gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(app->editor_buffer), &iter,
        gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(app->editor_buffer)));
    gtk_text_buffer_insert(GTK_TEXT_BUFFER(app->editor_buffer), &iter, "Row | Row\n", -1);
}

void on_table_insert_row_below_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    if (!app || !app->editor_buffer) return;
    
    GtkTextIter iter;
    gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(app->editor_buffer), &iter,
        gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(app->editor_buffer)));
    gtk_text_buffer_insert(GTK_TEXT_BUFFER(app->editor_buffer), &iter, "Row | Row\n", -1);
}

void on_table_insert_col_left_clicked(GtkWidget *widget, gpointer data) {
    (void)widget; (void)data;
    printf("Table: Insert Column Left clicked\n");
}

void on_table_insert_col_right_clicked(GtkWidget *widget, gpointer data) {
    (void)widget; (void)data;
    printf("Table: Insert Column Right clicked\n");
}

void on_table_delete_row_clicked(GtkWidget *widget, gpointer data) {
    (void)widget; (void)data;
    printf("Table: Delete Row clicked\n");
}

void on_table_delete_col_clicked(GtkWidget *widget, gpointer data) {
    (void)widget; (void)data;
    printf("Table: Delete Column clicked\n");
}

/* Window menu */
void on_window_new_clicked(GtkWidget *widget, gpointer data) {
    (void)widget; (void)data;
    
    GtkWidget *new_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(new_window), "IntelliEditor - New Instance");
    gtk_window_set_default_size(GTK_WINDOW(new_window), 800, 600);
    gtk_window_set_icon_name(GTK_WINDOW(new_window), "text-editor");
    
    GtkWidget *label = gtk_label_new("New IntelliEditor window");
    gtk_container_add(GTK_CONTAINER(new_window), label);
    
    g_signal_connect(new_window, "destroy", G_CALLBACK(gtk_widget_destroy), new_window);
    gtk_widget_show_all(new_window);
}

void on_window_fullscreen_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    if (!app || !app->window) return;
    
    GdkWindow *gdk_window = gtk_widget_get_window(app->window);
    if (gdk_window == NULL) return;
    
    if (gdk_window_get_state(gdk_window) & GDK_WINDOW_STATE_FULLSCREEN) {
        gtk_window_unfullscreen(GTK_WINDOW(app->window));
    } else {
        gtk_window_fullscreen(GTK_WINDOW(app->window));
    }
}

/* Help menu */
void on_help_contents_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    if (!app || !app->window) return;
    
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(app->window),
        GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
        "Help Contents");
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
        "IntelliEditor - Intelligent Text Editor\n\n"
        "Keyboard Shortcuts:\n"
        "Ctrl+N: New Document\n"
        "Ctrl+O: Open File\n"
        "Ctrl+S: Save\n"
        "Ctrl+Shift+S: Save As\n"
        "Ctrl+Z: Undo\n"
        "Ctrl+Y: Redo\n"
        "Ctrl+X: Cut\n"
        "Ctrl+C: Copy\n"
        "Ctrl+V: Paste\n"
        "Ctrl+A: Select All\n"
        "Ctrl+F: Find\n"
        "F7: Spellcheck\n"
        "F11: Fullscreen");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void on_help_about_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppWidgets *app = (AppWidgets *)data;
    if (!app || !app->window) return;
    
    GtkWidget *dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "IntelliEditor");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "1.0");
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), "Copyright © 2026");
    gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(dialog), GTK_LICENSE_GPL_3_0);
    gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), "http://localhost:8000");
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog),
        "An intelligent text editor with AI-powered suggestions and advanced formatting.");
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(app->window));
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}