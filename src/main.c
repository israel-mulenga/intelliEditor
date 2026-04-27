#include <gtk/gtk.h>
#include "../include/ui/window.h"
#include "editor/gap_buffer.h"

int main(int argc, char *argv[]) {
    GtkApplication *app;
    int status;

    GapBuffer *gb = gap_buffer_create(1024);
    app = gtk_application_new("com.example.intellieditor", G_APPLICATION_DEFAULT_FLAGS); // DEFAULT NONE IS DEPRECIATED
    g_signal_connect(app, "activate", G_CALLBACK(create_main_window), gb);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}