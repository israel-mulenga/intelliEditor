/**
 * nlp_highlighter.c
 * Real-time underlining with debounce in GtkSourceView.
 */

#define _GNU_SOURCE

#include "nlp/nlp_highlighter.h"
#include "nlp/nlp_engine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* INTERNAL STRUCTURE (opaque to other modules) */


struct NlpHighlighter {
    GtkSourceView *view;
    GtkTextBuffer *buffer;
    GtkTextTag    *tag_spelling;
    GtkTextTag    *tag_grammar;
    GtkTextTag    *tag_style;
    GtkTextTag    *tag_anglicism;
    GtkTextTag    *tag_repeat;
    guint          debounce_id;  /* 0 = no active timer */
};

#define DEBOUNCE_MS 500

/* GTK TAG CREATION */


/*
 * A GtkTextTag defines a visual style that can be applied to a text range.
 * We create it once and apply it dynamically.
 * "nlp-spelling" is a unique identifier in the tag table.
 */
static void create_tags(NlpHighlighter *hl) {
    GtkTextTagTable *table =
        gtk_text_buffer_get_tag_table(hl->buffer);

/* Local macro to avoid repetition */
#define GET_OR_CREATE(name, ...) \
    do { \
        GtkTextTag *existing = gtk_text_tag_table_lookup(table, name); \
        if (!existing) existing = gtk_text_buffer_create_tag( \
            hl->buffer, name, __VA_ARGS__); \
    } while(0)

    hl->tag_spelling = gtk_text_tag_table_lookup(table, "nlp-spelling");
    if (!hl->tag_spelling)
        hl->tag_spelling = gtk_text_buffer_create_tag(
            hl->buffer, "nlp-spelling",
            "underline",      PANGO_UNDERLINE_ERROR,
            "underline-rgba", &(GdkRGBA){0.9, 0.1, 0.1, 1.0},
            NULL);

    hl->tag_grammar = gtk_text_tag_table_lookup(table, "nlp-grammar");
    if (!hl->tag_grammar)
        hl->tag_grammar = gtk_text_buffer_create_tag(
            hl->buffer, "nlp-grammar",
            "underline",      PANGO_UNDERLINE_ERROR,
            "underline-rgba", &(GdkRGBA){1.0, 0.5, 0.0, 1.0},
            NULL);

    hl->tag_style = gtk_text_tag_table_lookup(table, "nlp-style");
    if (!hl->tag_style)
        hl->tag_style = gtk_text_buffer_create_tag(
            hl->buffer, "nlp-style",
            "underline",      PANGO_UNDERLINE_SINGLE,
            "underline-rgba", &(GdkRGBA){0.1, 0.4, 0.9, 1.0},
            NULL);

    hl->tag_anglicism = gtk_text_tag_table_lookup(table, "nlp-anglicism");
    if (!hl->tag_anglicism)
        hl->tag_anglicism = gtk_text_buffer_create_tag(
            hl->buffer, "nlp-anglicism",
            "underline",      PANGO_UNDERLINE_SINGLE,
            "underline-rgba", &(GdkRGBA){0.9, 0.8, 0.0, 1.0},
            NULL);

    hl->tag_repeat = gtk_text_tag_table_lookup(table, "nlp-repeat");
    if (!hl->tag_repeat)
        hl->tag_repeat = gtk_text_buffer_create_tag(
            hl->buffer, "nlp-repeat",
            "underline",      PANGO_UNDERLINE_SINGLE,
            "underline-rgba", &(GdkRGBA){0.6, 0.1, 0.8, 1.0},
            NULL);

#undef GET_OR_CREATE
}


/* LIFECYCLE */


NlpHighlighter *nlp_highlighter_create(GtkSourceView *view) {
    if (!view) return NULL;

    /* g_new0 = g_new + zero-initialized fields (debounce_id = 0) */
    NlpHighlighter *hl = g_new0(NlpHighlighter, 1);
    hl->view   = view;
    hl->buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));

    create_tags(hl);

    fprintf(stdout, "[NLP Highlighter] Initialized.\n");
    return hl;
}

void nlp_highlighter_destroy(NlpHighlighter *hl) {
    if (!hl) return;
    if (hl->debounce_id != 0) {
        g_source_remove(hl->debounce_id);
    }
    g_free(hl);
}


/* CLEAR HIGHLIGHTS */


void nlp_highlighter_clear(NlpHighlighter *hl) {
    if (!hl || !hl->buffer) return;

    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(hl->buffer, &start, &end);

    if (hl->tag_spelling)
        gtk_text_buffer_remove_tag(hl->buffer, hl->tag_spelling, &start, &end);
    if (hl->tag_grammar)
        gtk_text_buffer_remove_tag(hl->buffer, hl->tag_grammar,  &start, &end);
    if (hl->tag_style)
        gtk_text_buffer_remove_tag(hl->buffer, hl->tag_style,    &start, &end);
    if (hl->tag_anglicism)
        gtk_text_buffer_remove_tag(hl->buffer, hl->tag_anglicism,&start, &end);
    if (hl->tag_repeat)
        gtk_text_buffer_remove_tag(hl->buffer, hl->tag_repeat,   &start, &end);
}


/* APPLY HIGHLIGHTS */


static void apply_highlights(NlpHighlighter *hl,
                               const char     *full_text,
                               const NlpErrorList *errors) {
    for (int i = 0; i < errors->count; i++) {
        NlpError *e = &errors->items[i];
        if (e->start < 0 || e->length <= 0) continue;

        /*
         * Convert bytes → Unicode characters.
         * NlpError uses bytes, GtkTextBuffer uses characters.
         * g_utf8_pointer_to_offset() performs the conversion.
         */
        glong char_start = g_utf8_pointer_to_offset(
            full_text, full_text + e->start);
        glong char_end = g_utf8_pointer_to_offset(
            full_text, full_text + e->start + e->length);

        GtkTextIter iter_start, iter_end;
        gtk_text_buffer_get_iter_at_offset(
            hl->buffer, &iter_start, (gint)char_start);
        gtk_text_buffer_get_iter_at_offset(
            hl->buffer, &iter_end,   (gint)char_end);

        GtkTextTag *tag = NULL;
        switch (e->type) {
            case NLP_ERR_SPELLING:  tag = hl->tag_spelling;  break;
            case NLP_ERR_GRAMMAR:   tag = hl->tag_grammar;   break;
            case NLP_ERR_STYLE:     tag = hl->tag_style;     break;
            case NLP_ERR_ANGLICISM: tag = hl->tag_anglicism; break;
            case NLP_ERR_REPEAT:    tag = hl->tag_repeat;    break;
            default:                tag = hl->tag_spelling;  break;
        }

        if (tag)
            gtk_text_buffer_apply_tag(
                hl->buffer, tag, &iter_start, &iter_end);
    }
}

/* DEBOUNCE AND CHECKING */

typedef struct { NlpHighlighter *hl; } DebounceData;

static gboolean run_check(gpointer data) {
    DebounceData   *dd = (DebounceData *)data;
    NlpHighlighter *hl = dd->hl;
    g_free(dd);

    hl->debounce_id = 0;

    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(hl->buffer, &start, &end);
    char *text = gtk_text_buffer_get_text(
        hl->buffer, &start, &end, FALSE);

    if (!text || strlen(text) < 2) {
        g_free(text);
        return G_SOURCE_REMOVE;
    }

    NlpErrorList errors;
    int n = nlp_check_all_sync(text, &errors);

    nlp_highlighter_clear(hl);

    if (n > 0)
        apply_highlights(hl, text, &errors);

    nlp_error_list_free(&errors);
    g_free(text);

    return G_SOURCE_REMOVE;
}

void nlp_highlighter_on_text_changed(GtkTextBuffer *buffer,
                                      gpointer       user_data) {
    (void)buffer;
    NlpHighlighter *hl = (NlpHighlighter *)user_data;
    if (!hl) return;

    /* Cancel the previous timer if active */
    if (hl->debounce_id != 0) {
        g_source_remove(hl->debounce_id);
        hl->debounce_id = 0;
    }

    /* Start a new 500ms timer */
    DebounceData *dd = g_new(DebounceData, 1);
    dd->hl = hl;
    hl->debounce_id = g_timeout_add(DEBOUNCE_MS, run_check, dd);
}

void nlp_highlighter_run_now(NlpHighlighter *hl) {
    if (!hl) return;
    if (hl->debounce_id != 0) {
        g_source_remove(hl->debounce_id);
        hl->debounce_id = 0;
    }
    DebounceData *dd = g_new(DebounceData, 1);
    dd->hl = hl;
    run_check(dd);
}