#include "../../include/ui/toolbar.h"
#include "../../include/ui/callbacks.h"
#include "../../include/ui/window.h"

// ======================================
// MODULE: TOOLBAR
// ROLE: Create action buttons
// ======================================

GtkWidget* create_toolbar(AppWidgets *app_widgets) {
    GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_name(toolbar, "toolbar");

    GtkWidget *btn = gtk_button_new_with_label("Correct");
    gtk_widget_set_name(btn, "toolbar-button");
    g_signal_connect(btn, "clicked", G_CALLBACK(on_correct_clicked), app_widgets);

    gtk_box_pack_start(GTK_BOX(toolbar), btn, FALSE, FALSE, 0);

    return toolbar;
}
