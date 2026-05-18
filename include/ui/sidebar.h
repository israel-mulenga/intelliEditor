#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <gtk/gtk.h>

GtkWidget *create_sidebar(void);

void sidebar_set_summary(GtkWidget *sidebar, const char *text);

void sidebar_set_rules_file(GtkWidget *sidebar, const char *filename);

GtkListBox *sidebar_get_rules_list(GtkWidget *sidebar);

#endif
