#ifndef WINDOW_H
#define WINDOW_H

#include <gtk/gtk.h>
#include <hunspell/hunspell.h>

typedef struct _AppWidgets {
    GtkWidget *stack;
    GtkApplication *app;
    GtkTextBuffer *editor_buffer;
    GtkWidget *sidebar;
    Hunhandle *hun_en;
    Hunhandle *hun_fr;
} AppWidgets;

void create_main_window(GtkApplication *app, gpointer user_data);

#endif
