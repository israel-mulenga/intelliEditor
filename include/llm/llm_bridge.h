#ifndef LLM_BRIDGE_H
#define LLM_BRIDGE_H

#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>

typedef struct {
    GtkSourceView *source_view;
    GtkWidget     *statusbar;
    GtkWidget     *sidebar;
} AppContext;

void llm_bridge_on_write_clicked(GtkWidget *widget, gpointer user_data);
void llm_bridge_on_gramma_clicked(GtkWidget *widget, gpointer user_data);
char *llm_bridge_get_selected_text(GtkSourceView *view);
void llm_bridge_replace_selection(GtkSourceView *view,
                                  const char *new_text);
void llm_bridge_show_status(GtkWidget *statusbar,
                            const char *message);

#endif // LLM_BRIDGE_H
