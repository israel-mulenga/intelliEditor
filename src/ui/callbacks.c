#include "ui/callbacks.h"
#include "ui/window.h"
#include <stdio.h>

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