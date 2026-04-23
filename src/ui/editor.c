#include "../../include/ui/editor.h"
#include <gtksourceview/gtksource.h>
#include "../../include/ui/window.h"

/* =========================================================
   CRÉATION DE L'ÉDITEUR
   Style Word-like (simple et propre)
   ========================================================= */
GtkWidget *create_editor(void) {

    /* Buffer source view pour l'édition avec coloration syntaxe */
    GtkSourceBuffer *buffer = gtk_source_buffer_new(NULL);
    gtk_source_buffer_set_language(buffer, NULL); // Pas de coloration syntaxe pour le moment

    /* Zone d'édition */
    GtkWidget *view = gtk_text_view_new_with_buffer(GTK_TEXT_BUFFER(buffer));
    gtk_widget_set_name(view, "editor-textview");

    /* ================= STYLE WORD ================= */

    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD);

    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(view), 40);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(view), 40);

    gtk_text_view_set_pixels_above_lines(GTK_TEXT_VIEW(view), 5);
    gtk_text_view_set_pixels_below_lines(GTK_TEXT_VIEW(view), 5);

    return view;
}