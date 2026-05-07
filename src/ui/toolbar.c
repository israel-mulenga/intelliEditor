#include "ui/toolbar.h"
#include "ui/callbacks.h"
#include "ui/window.h"
#include "llm/llm_bridge.h"

// ======================================
// MODULE: TOOLBAR
// ROLE: Create action buttons with icons
// ======================================

GtkWidget* create_toolbar(AppWidgets *app_widgets) {
    GtkWidget *menu_bar = gtk_menu_bar_new();
    gtk_widget_set_name(menu_bar, "toolbar");

    // Créer un item File au menu
    GtkWidget *file_item = gtk_menu_item_new_with_label("File");
    GtkWidget *file_menu = gtk_menu_new();

    // Créer l'élément "Import" avec icône
    GtkWidget *import_item = gtk_menu_item_new();
    GtkWidget *import_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);  // Boîte horizontale avec espacement
    GtkWidget *import_image = gtk_image_new_from_icon_name("document-open", GTK_ICON_SIZE_MENU);  // Icône
    GtkWidget *import_label = gtk_label_new("Import...");
    gtk_box_pack_start(GTK_BOX(import_box), import_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(import_box), import_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(import_item), import_box);
    g_signal_connect(import_item, "activate", G_CALLBACK(on_file_import_clicked), app_widgets);

    // Créer l'élément "Save" avec icône
    GtkWidget *save_item = gtk_menu_item_new();
    GtkWidget *save_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *save_image = gtk_image_new_from_icon_name("document-save", GTK_ICON_SIZE_MENU);
    GtkWidget *save_label = gtk_label_new("Save");
    gtk_box_pack_start(GTK_BOX(save_box), save_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(save_box), save_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(save_item), save_box);
    g_signal_connect(save_item, "activate", G_CALLBACK(on_file_save_clicked), app_widgets);

    // Créer l'élément "Save As" avec icône
    GtkWidget *save_as_item = gtk_menu_item_new();
    GtkWidget *save_as_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *save_as_image = gtk_image_new_from_icon_name("document-save-as", GTK_ICON_SIZE_MENU);
    GtkWidget *save_as_label = gtk_label_new("Save As...");
    gtk_box_pack_start(GTK_BOX(save_as_box), save_as_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(save_as_box), save_as_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(save_as_item), save_as_box);
    g_signal_connect(save_as_item, "activate", G_CALLBACK(on_file_save_as_clicked), app_widgets);

    // Ajouter les éléments au menu
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), import_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_as_item);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_item);

    GtkWidget *edit_item = gtk_menu_item_new_with_label("Edit");
    GtkWidget *edit_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_item), edit_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), edit_item);

    return menu_bar;
}