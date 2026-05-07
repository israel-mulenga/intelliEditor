#include "ui/toolbar.h"
#include "ui/callbacks.h"
#include "ui/window.h"
#include "llm/llm_bridge.h"
#include <gdk/gdkkeysyms.h>

// ======================================
// MODULE: TOOLBAR
// ROLE: Create action buttons with icons
// ======================================

GtkWidget* create_toolbar(AppWidgets *app_widgets, GtkAccelGroup *accel_group) {
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
    gtk_widget_add_accelerator(import_item, "activate", accel_group,
                               GDK_KEY_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    // Créer l'élément "Save" avec icône
    GtkWidget *save_item = gtk_menu_item_new();
    GtkWidget *save_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *save_image = gtk_image_new_from_icon_name("document-save", GTK_ICON_SIZE_MENU);
    GtkWidget *save_label = gtk_label_new("Save");
    gtk_box_pack_start(GTK_BOX(save_box), save_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(save_box), save_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(save_item), save_box);
    g_signal_connect(save_item, "activate", G_CALLBACK(on_file_save_clicked), app_widgets);
    gtk_widget_add_accelerator(save_item, "activate", accel_group,
                               GDK_KEY_s, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    // Créer l'élément "Save As" avec icône
    GtkWidget *save_as_item = gtk_menu_item_new();
    GtkWidget *save_as_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *save_as_image = gtk_image_new_from_icon_name("document-save-as", GTK_ICON_SIZE_MENU);
    GtkWidget *save_as_label = gtk_label_new("Save As...");
    gtk_box_pack_start(GTK_BOX(save_as_box), save_as_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(save_as_box), save_as_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(save_as_item), save_as_box);
    g_signal_connect(save_as_item, "activate", G_CALLBACK(on_file_save_as_clicked), app_widgets);
    gtk_widget_add_accelerator(save_as_item, "activate", accel_group,
                               GDK_KEY_s, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);

    // Ajouter les éléments au menu
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), import_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_as_item);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_item);






    // Créer un item Edit au menu
    GtkWidget *edit_item = gtk_menu_item_new_with_label("Edit");
    GtkWidget *edit_menu = gtk_menu_new();
    
     // Créer l'élément "Undo" avec icône
    GtkWidget *undo_item = gtk_menu_item_new();
    GtkWidget *undo_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);  // Boîte horizontale avec espacement
    GtkWidget *undo_image = gtk_image_new_from_icon_name("edit-undo", GTK_ICON_SIZE_MENU);  // Icône
    GtkWidget *undo_label = gtk_label_new("Undo");
    gtk_box_pack_start(GTK_BOX(undo_box), undo_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(undo_box), undo_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(undo_item), undo_box);
    g_signal_connect(undo_item, "activate", G_CALLBACK(on_edit_undo_clicked), app_widgets); // Connecte le signal d'activation au callback undo
    gtk_widget_add_accelerator(undo_item, "activate", accel_group,
                               GDK_KEY_z, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), undo_item);

    // Créer l'élément "Redo" avec icône
    GtkWidget *redo_item = gtk_menu_item_new();
    GtkWidget *redo_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);  // Boîte horizontale avec espacement
    GtkWidget *redo_image = gtk_image_new_from_icon_name("edit-redo", GTK_ICON_SIZE_MENU);
    GtkWidget *redo_label = gtk_label_new("Redo");
    gtk_box_pack_start(GTK_BOX(redo_box), redo_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(redo_box), redo_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(redo_item), redo_box);
    g_signal_connect(redo_item, "activate", G_CALLBACK(on_edit_redo_clicked), app_widgets);
    gtk_widget_add_accelerator(redo_item, "activate", accel_group,
                               GDK_KEY_y, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), redo_item);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_item), edit_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), edit_item);





     // Créer un item View au menu
    GtkWidget *view_item = gtk_menu_item_new_with_label("View");
    GtkWidget *view_menu = gtk_menu_new();

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_item), view_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), view_item);





     // Créer un item Insert au menu
    GtkWidget *insert_item = gtk_menu_item_new_with_label("Insert");
    GtkWidget *insert_menu = gtk_menu_new();

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(insert_item), insert_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), insert_item);





     // Créer un item Format au menu
    GtkWidget *format_item = gtk_menu_item_new_with_label("Formatt");
    GtkWidget *format_menu = gtk_menu_new();

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(format_item), format_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), format_item);





     // Créer un item Tools au menu
    GtkWidget *tools_item = gtk_menu_item_new_with_label("Tools");
    GtkWidget *tools_menu = gtk_menu_new();

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(tools_item), tools_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), tools_item);





     // Créer un item Table au menu
    GtkWidget *Table_item = gtk_menu_item_new_with_label("Table");
    GtkWidget *Tablee_menu = gtk_menu_new();

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(Table_item), Tablee_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), Table_item);




     // Créer un item Window au menu
    GtkWidget *window_item = gtk_menu_item_new_with_label("Window");
    GtkWidget *window_menu = gtk_menu_new();


    gtk_menu_item_set_submenu(GTK_MENU_ITEM(window_item), window_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), window_item);



     // Créer un item Help au menu
    GtkWidget *help_item = gtk_menu_item_new_with_label("Help");
    GtkWidget *help_menu = gtk_menu_new();
    

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(help_item), help_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), help_item);

    return menu_bar;
}