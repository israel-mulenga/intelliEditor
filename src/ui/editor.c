#include "ui/editor.h"
#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
// ======================================
// MODULE: EDITOR
// ROLE: Create the text editing area
// ======================================

GtkWidget* create_editor() {

    GtkSourceBuffer *buffer;
    GtkWidget *view;

    // Create a text buffer (stores the text)
    buffer = gtk_source_buffer_new(NULL);

    // Create the text editor view
    view = gtk_source_view_new_with_buffer(buffer);
    gtk_widget_set_name(view, "editor-view");

    // Show line numbers on the left
    gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(view), TRUE);

    // Make the editor expand in the window
    gtk_widget_set_hexpand(view, TRUE);
    gtk_widget_set_vexpand(view, TRUE);

    return view;
}
