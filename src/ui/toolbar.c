#include "ui/toolbar.h"
#include "ui/callbacks.h"
#include "ui/window.h"
#include "llm/llm_bridge.h"
#include <gtk/gtk.h>
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
    gtk_widget_add_accelerator(import_item, "activate", accel_group, GDK_KEY_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    // Créer l'élément "Save" avec icône
    GtkWidget *save_item = gtk_menu_item_new();
    GtkWidget *save_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *save_image = gtk_image_new_from_icon_name("document-save", GTK_ICON_SIZE_MENU);
    GtkWidget *save_label = gtk_label_new("Save");
    gtk_box_pack_start(GTK_BOX(save_box), save_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(save_box), save_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(save_item), save_box);
    g_signal_connect(save_item, "activate", G_CALLBACK(on_file_save_clicked), app_widgets);
    gtk_widget_add_accelerator(save_item, "activate", accel_group, GDK_KEY_s, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    // Créer l'élément "Save As" avec icône
    GtkWidget *save_as_item = gtk_menu_item_new();
    GtkWidget *save_as_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *save_as_image = gtk_image_new_from_icon_name("document-save-as", GTK_ICON_SIZE_MENU);
    GtkWidget *save_as_label = gtk_label_new("Save As...");
    gtk_box_pack_start(GTK_BOX(save_as_box), save_as_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(save_as_box), save_as_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(save_as_item), save_as_box);
    g_signal_connect(save_as_item, "activate", G_CALLBACK(on_file_save_as_clicked), app_widgets);

    GdkModifierType modifiers;
    /* =========================================================
   RACCOURCI CLAVIER : CTRL + SHIFT + S
   Permet d'ouvrir "Save As..."
   
   GDK_CONTROL_MASK = touche Ctrl
   GDK_SHIFT_MASK   = touche Shift

   Le cast (GdkModifierType) est nécessaire
   car l'opérateur "|" retourne un int.
   ========================================================= */
    gtk_widget_add_accelerator(
    save_as_item,
    "activate",
    accel_group,
    GDK_KEY_s,
    (GdkModifierType)(GDK_CONTROL_MASK | GDK_SHIFT_MASK),
    GTK_ACCEL_VISIBLE
    );

    // Créer l'élément "New Page" avec icône
    GtkWidget *new_page_item = gtk_menu_item_new();
    GtkWidget *new_page_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *new_page_image = gtk_image_new_from_icon_name("view-paged", GTK_ICON_SIZE_MENU);
    GtkWidget *new_page_label = gtk_label_new("New Page");
    gtk_box_pack_start(GTK_BOX(new_page_box), new_page_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(new_page_box), new_page_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(new_page_item), new_page_box);
    g_signal_connect(new_page_item, "activate", G_CALLBACK(on_file_new_page_clicked), app_widgets);
    gtk_widget_add_accelerator(new_page_item, "activate", accel_group, GDK_KEY_p, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);

    // Créer l'élément "New Document" avec icône
    GtkWidget *new_document_item = gtk_menu_item_new();
    GtkWidget *new_document_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *new_document_image = gtk_image_new_from_icon_name("document-new", GTK_ICON_SIZE_MENU);
    GtkWidget *new_document_label = gtk_label_new("New Document");
    gtk_box_pack_start(GTK_BOX(new_document_box), new_document_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(new_document_box), new_document_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(new_document_item), new_document_box);
    g_signal_connect(new_document_item, "activate", G_CALLBACK(on_file_new_document_clicked), app_widgets);
    gtk_widget_add_accelerator(new_document_item, "activate", accel_group, GDK_KEY_n, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);

    // Ajouter les éléments au menu
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), import_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), new_page_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), new_document_item);
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
    gtk_widget_add_accelerator(undo_item, "activate", accel_group, GDK_KEY_z, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

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
    gtk_widget_add_accelerator(redo_item, "activate", accel_group, GDK_KEY_y, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), redo_item);


     // Créer l'élément "Cut" avec icône
    GtkWidget *cut_item = gtk_menu_item_new();
    GtkWidget *cut_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *cut_image = gtk_image_new_from_icon_name("edit-cut", GTK_ICON_SIZE_MENU); // Corrigé ici
    GtkWidget *cut_label = gtk_label_new("Cut");
    gtk_box_pack_start(GTK_BOX(cut_box), cut_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(cut_box), cut_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(cut_item), cut_box);
    g_signal_connect(cut_item, "activate", G_CALLBACK(on_edit_cut_clicked), app_widgets);
    gtk_widget_add_accelerator(cut_item, "activate", accel_group, GDK_KEY_x, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), cut_item);


     // Créer l'élément "Copy" avec icône
    GtkWidget *copy_item = gtk_menu_item_new();
    GtkWidget *copy_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *copy_image = gtk_image_new_from_icon_name("edit-copy", GTK_ICON_SIZE_MENU);
    GtkWidget *copy_label = gtk_label_new("Copy");
    
    gtk_box_pack_start(GTK_BOX(copy_box), copy_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(copy_box), copy_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(copy_item), copy_box);
    
    g_signal_connect(copy_item, "activate", G_CALLBACK(on_edit_copy_clicked), app_widgets);
    gtk_widget_add_accelerator(copy_item, "activate", accel_group, GDK_KEY_c, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), copy_item);

    

     // Créer l'élément "Paste" avec icône
    GtkWidget *paste_item = gtk_menu_item_new();
    GtkWidget *paste_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *paste_image = gtk_image_new_from_icon_name("edit-paste", GTK_ICON_SIZE_MENU);
    GtkWidget *paste_label = gtk_label_new("Paste");
    
    gtk_box_pack_start(GTK_BOX(paste_box), paste_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(paste_box), paste_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(paste_item), paste_box);
    
    g_signal_connect(paste_item, "activate", G_CALLBACK(on_edit_paste_clicked), app_widgets);
    gtk_widget_add_accelerator(paste_item, "activate", accel_group, GDK_KEY_v, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), paste_item);


     // Créer l'élément "Select All" avec icône
    GtkWidget *selectall_item = gtk_menu_item_new();
    GtkWidget *selectall_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *selectall_image = gtk_image_new_from_icon_name("edit-select-all", GTK_ICON_SIZE_MENU);
    GtkWidget *selectall_label = gtk_label_new("Select All");
    
    gtk_box_pack_start(GTK_BOX(selectall_box), selectall_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(selectall_box), selectall_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(selectall_item), selectall_box);
    
    g_signal_connect(selectall_item, "activate", G_CALLBACK(on_edit_select_all_clicked), app_widgets);
    gtk_widget_add_accelerator(selectall_item, "activate", accel_group, GDK_KEY_a, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), selectall_item);


   // Créer l'élément "Find" avec icône
    GtkWidget *find_item = gtk_menu_item_new();
    GtkWidget *find_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *find_image = gtk_image_new_from_icon_name("edit-find", GTK_ICON_SIZE_MENU);
    GtkWidget *find_label = gtk_label_new("Find...");
    
    gtk_box_pack_start(GTK_BOX(find_box), find_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(find_box), find_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(find_item), find_box);
    
    g_signal_connect(find_item, "activate", G_CALLBACK(on_edit_find_clicked), app_widgets);
    gtk_widget_add_accelerator(find_item, "activate", accel_group, GDK_KEY_f, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), find_item);

    // Créer l'élément "Rephrase" (LLM) avec icône
    GtkWidget *rephrase_item = gtk_menu_item_new();
    GtkWidget *rephrase_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *rephrase_image = gtk_image_new_from_icon_name("document-revert-symbolic", GTK_ICON_SIZE_MENU);
    GtkWidget *rephrase_label = gtk_label_new("Rephrase");
    gtk_box_pack_start(GTK_BOX(rephrase_box), rephrase_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(rephrase_box), rephrase_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(rephrase_item), rephrase_box);
    g_signal_connect(rephrase_item, "activate", G_CALLBACK(on_llm_rewrite_clicked), app_widgets);
    gtk_widget_add_accelerator(rephrase_item, "activate", accel_group, GDK_KEY_r, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), rephrase_item);

    // Créer l'élément "Grammar Check" (LLM) avec icône
    GtkWidget *grammar_item = gtk_menu_item_new();
    GtkWidget *grammar_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *grammar_image = gtk_image_new_from_icon_name("document-properties-symbolic", GTK_ICON_SIZE_MENU);
    GtkWidget *grammar_label = gtk_label_new("Grammar Check");
    gtk_box_pack_start(GTK_BOX(grammar_box), grammar_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(grammar_box), grammar_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(grammar_item), grammar_box);
    g_signal_connect(grammar_item, "activate", G_CALLBACK(on_llm_grammar_clicked), app_widgets);
    gtk_widget_add_accelerator(grammar_item, "activate", accel_group, GDK_KEY_g, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), grammar_item);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_item), edit_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), edit_item);




     // Créer un item View au menu
    GtkWidget *view_item = gtk_menu_item_new_with_label("View");
    GtkWidget *view_menu = gtk_menu_new();

    GtkWidget *horizontal_ruler_item = gtk_check_menu_item_new_with_label("Horizontal Ruler");
    g_signal_connect(horizontal_ruler_item, "toggled",
        G_CALLBACK(on_view_toggle_horizontal_ruler_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), horizontal_ruler_item);

    GtkWidget *vertical_ruler_item = gtk_check_menu_item_new_with_label("Vertical Ruler");
    g_signal_connect(vertical_ruler_item, "toggled",
        G_CALLBACK(on_view_toggle_vertical_ruler_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), vertical_ruler_item);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_item), view_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), view_item);





     // Créer un item Insert au menu
    GtkWidget *insert_item = gtk_menu_item_new_with_label("Insert");
    GtkWidget *insert_menu = gtk_menu_new();

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(insert_item), insert_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), insert_item);





     // Créer un item Format au menu
    GtkWidget *format_item = gtk_menu_item_new_with_label("Format");
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