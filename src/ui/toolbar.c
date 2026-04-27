#include "ui/toolbar.h"
#include "ui/callbacks.h"
#include "ui/window.h"

// ======================================
// MODULE: TOOLBAR
// ROLE: Create action buttons
// ======================================

GtkWidget* create_toolbar(AppWidgets *app_widgets) {
    GtkWidget *menu_bar = gtk_menu_bar_new();
    gtk_widget_set_name(menu_bar, "toolbar");

    GtkWidget *file_item = gtk_menu_item_new_with_label("File");
    GtkWidget *file_menu = gtk_menu_new();
    GtkWidget *import_item = gtk_menu_item_new_with_label("Import...");
    GtkWidget *save_item = gtk_menu_item_new_with_label("Save");
    GtkWidget *save_as_item = gtk_menu_item_new_with_label("Save As...");

    g_signal_connect(import_item, "activate", G_CALLBACK(on_file_import_clicked), app_widgets);
    g_signal_connect(save_item, "activate", G_CALLBACK(on_file_save_clicked), app_widgets);
    g_signal_connect(save_as_item, "activate", G_CALLBACK(on_file_save_as_clicked), app_widgets);

    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), import_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_as_item);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_item);

    return menu_bar;
}
