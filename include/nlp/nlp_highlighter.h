
#ifndef NLP_HIGHLIGHTER_H
#define NLP_HIGHLIGHTER_H

#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include <stdbool.h>

/* Opaque type — other modules only see a pointer */
typedef struct NlpHighlighter NlpHighlighter;

/**
 * nlp_highlighter_create() — initialize the highlighter.
 * Creates the GtkTextTag for each underline color.
 * @return allocated pointer, NULL on failure
 */
NlpHighlighter *nlp_highlighter_create(GtkSourceView *view);

/** nlp_highlighter_destroy() — free all resources. */
void nlp_highlighter_destroy(NlpHighlighter *hl);

/**
 * nlp_highlighter_on_text_changed()
 * Connect to the "changed" signal of a GtkTextBuffer.
 * Handles a 500ms debounce then triggers a spelling check.
 *
 * g_signal_connect(buffer, "changed",
 *                  G_CALLBACK(nlp_highlighter_on_text_changed), hl);
 */
void nlp_highlighter_on_text_changed(GtkTextBuffer *buffer,
                                      gpointer       user_data);

/** nlp_highlighter_clear() — clear all underlines. */
void nlp_highlighter_clear(NlpHighlighter *hl);

/**
 * nlp_highlighter_run_now() — force an immediate check.
 * Does not wait for the debounce. Useful when opening a file.
 */
void nlp_highlighter_run_now(NlpHighlighter *hl);

#endif /* NLP_HIGHLIGHTER_H */