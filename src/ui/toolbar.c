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

    // ======================================
    // 1. FILE MENU
    // ======================================
    GtkWidget *file_item = gtk_menu_item_new_with_label("File");
    GtkWidget *file_menu = gtk_menu_new();

    GtkWidget *import_item = gtk_menu_item_new();
    GtkWidget *import_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *import_image = gtk_image_new_from_icon_name("document-open", GTK_ICON_SIZE_MENU);
    GtkWidget *import_label = gtk_label_new("Import...");
    gtk_box_pack_start(GTK_BOX(import_box), import_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(import_box), import_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(import_item), import_box);
    g_signal_connect(import_item, "activate", G_CALLBACK(on_file_import_clicked), app_widgets);
    gtk_widget_add_accelerator(import_item, "activate", accel_group, GDK_KEY_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    GtkWidget *save_item = gtk_menu_item_new();
    GtkWidget *save_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *save_image = gtk_image_new_from_icon_name("document-save", GTK_ICON_SIZE_MENU);
    GtkWidget *save_label = gtk_label_new("Save");
    gtk_box_pack_start(GTK_BOX(save_box), save_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(save_box), save_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(save_item), save_box);
    g_signal_connect(save_item, "activate", G_CALLBACK(on_file_save_clicked), app_widgets);
    gtk_widget_add_accelerator(save_item, "activate", accel_group, GDK_KEY_s, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    GtkWidget *save_as_item = gtk_menu_item_new();
    GtkWidget *save_as_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *save_as_image = gtk_image_new_from_icon_name("document-save-as", GTK_ICON_SIZE_MENU);
    GtkWidget *save_as_label = gtk_label_new("Save As...");
    gtk_box_pack_start(GTK_BOX(save_as_box), save_as_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(save_as_box), save_as_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(save_as_item), save_as_box);
    g_signal_connect(save_as_item, "activate", G_CALLBACK(on_file_save_as_clicked), app_widgets);
    gtk_widget_add_accelerator(save_as_item, "activate", accel_group, GDK_KEY_s, (GdkModifierType)(GDK_CONTROL_MASK | GDK_SHIFT_MASK), GTK_ACCEL_VISIBLE);

    GtkWidget *new_page_item = gtk_menu_item_new();
    GtkWidget *new_page_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *new_page_image = gtk_image_new_from_icon_name("view-paged", GTK_ICON_SIZE_MENU);
    GtkWidget *new_page_label = gtk_label_new("New Page");
    gtk_box_pack_start(GTK_BOX(new_page_box), new_page_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(new_page_box), new_page_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(new_page_item), new_page_box);
    g_signal_connect(new_page_item, "activate", G_CALLBACK(on_file_new_page_clicked), app_widgets);
    gtk_widget_add_accelerator(new_page_item, "activate", accel_group, GDK_KEY_p, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);

    GtkWidget *new_document_item = gtk_menu_item_new();
    GtkWidget *new_document_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *new_document_image = gtk_image_new_from_icon_name("document-new", GTK_ICON_SIZE_MENU);
    GtkWidget *new_document_label = gtk_label_new("New Document");
    gtk_box_pack_start(GTK_BOX(new_document_box), new_document_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(new_document_box), new_document_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(new_document_item), new_document_box);
    g_signal_connect(new_document_item, "activate", G_CALLBACK(on_file_new_document_clicked), app_widgets);
    gtk_widget_add_accelerator(new_document_item, "activate", accel_group, GDK_KEY_n, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);

    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), import_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), new_page_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), new_document_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_as_item);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_item);

    // ======================================
    // 2. EDIT MENU
    // ======================================
    GtkWidget *edit_item = gtk_menu_item_new_with_label("Edit");
    GtkWidget *edit_menu = gtk_menu_new();
    
    GtkWidget *undo_item = gtk_menu_item_new();
    GtkWidget *undo_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *undo_image = gtk_image_new_from_icon_name("edit-undo", GTK_ICON_SIZE_MENU);
    GtkWidget *undo_label = gtk_label_new("Undo");
    gtk_box_pack_start(GTK_BOX(undo_box), undo_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(undo_box), undo_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(undo_item), undo_box);
    g_signal_connect(undo_item, "activate", G_CALLBACK(on_edit_undo_clicked), app_widgets);
    gtk_widget_add_accelerator(undo_item, "activate", accel_group, GDK_KEY_z, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), undo_item);

    GtkWidget *redo_item = gtk_menu_item_new();
    GtkWidget *redo_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *redo_image = gtk_image_new_from_icon_name("edit-redo", GTK_ICON_SIZE_MENU);
    GtkWidget *redo_label = gtk_label_new("Redo");
    gtk_box_pack_start(GTK_BOX(redo_box), redo_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(redo_box), redo_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(redo_item), redo_box);
    g_signal_connect(redo_item, "activate", G_CALLBACK(on_edit_redo_clicked), app_widgets);
    gtk_widget_add_accelerator(redo_item, "activate", accel_group, GDK_KEY_y, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), redo_item);

    GtkWidget *cut_item = gtk_menu_item_new();
    GtkWidget *cut_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *cut_image = gtk_image_new_from_icon_name("edit-cut", GTK_ICON_SIZE_MENU);
    GtkWidget *cut_label = gtk_label_new("Cut");
    gtk_box_pack_start(GTK_BOX(cut_box), cut_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(cut_box), cut_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(cut_item), cut_box);
    g_signal_connect(cut_item, "activate", G_CALLBACK(on_edit_cut_clicked), app_widgets);
    gtk_widget_add_accelerator(cut_item, "activate", accel_group, GDK_KEY_x, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), cut_item);

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

    GtkWidget *rephrase_item = gtk_menu_item_new();
    GtkWidget *rephrase_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *rephrase_image = gtk_image_new_from_icon_name("document-edit", GTK_ICON_SIZE_MENU);
    GtkWidget *rephrase_label = gtk_label_new("Rephrase");
    gtk_box_pack_start(GTK_BOX(rephrase_box), rephrase_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(rephrase_box), rephrase_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(rephrase_item), rephrase_box);
    g_signal_connect(rephrase_item, "activate", G_CALLBACK(on_llm_rewrite_clicked), app_widgets);
    gtk_widget_add_accelerator(rephrase_item, "activate", accel_group, GDK_KEY_r, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), rephrase_item);

    GtkWidget *grammar_item = gtk_menu_item_new();
    GtkWidget *grammar_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *grammar_image = gtk_image_new_from_icon_name("dialog-information", GTK_ICON_SIZE_MENU);
    GtkWidget *grammar_label = gtk_label_new("Grammar Check");
    gtk_box_pack_start(GTK_BOX(grammar_box), grammar_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(grammar_box), grammar_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(grammar_item), grammar_box);
    g_signal_connect(grammar_item, "activate", G_CALLBACK(on_llm_grammar_clicked), app_widgets);
    gtk_widget_add_accelerator(grammar_item, "activate", accel_group, GDK_KEY_g, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), grammar_item);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_item), edit_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), edit_item);

    // ======================================
    // 3. VIEW MENU
    // ======================================
    GtkWidget *view_item = gtk_menu_item_new_with_label("View");
    GtkWidget *view_menu = gtk_menu_new();

    GtkWidget *horizontal_ruler_item = gtk_check_menu_item_new_with_label("Horizontal Ruler");
    g_signal_connect(horizontal_ruler_item, "toggled", G_CALLBACK(on_view_toggle_horizontal_ruler_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), horizontal_ruler_item);

    GtkWidget *vertical_ruler_item = gtk_check_menu_item_new_with_label("Vertical Ruler");
    g_signal_connect(vertical_ruler_item, "toggled", G_CALLBACK(on_view_toggle_vertical_ruler_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), vertical_ruler_item);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_item), view_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), view_item);

    // ======================================
    // 4. INSERT MENU
    // ======================================
    GtkWidget *insert_item = gtk_menu_item_new_with_label("Insert");
    GtkWidget *insert_menu = gtk_menu_new();

    GtkWidget *image_item = gtk_menu_item_new_with_label("Image...");
    g_signal_connect(image_item, "activate", G_CALLBACK(on_insert_image_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(insert_menu), image_item);
    
    GtkWidget *table_insert_item = gtk_menu_item_new_with_label("Table...");
    g_signal_connect(table_insert_item, "activate", G_CALLBACK(on_insert_table_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(insert_menu), table_insert_item);

    GtkWidget *separator1 = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(insert_menu), separator1);

    GtkWidget *date_item = gtk_menu_item_new_with_label("Date & Time");
    g_signal_connect(date_item, "activate", G_CALLBACK(on_insert_date_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(insert_menu), date_item);

    GtkWidget *page_break_item = gtk_menu_item_new_with_label("Page Break");
    g_signal_connect(page_break_item, "activate", G_CALLBACK(on_insert_page_break_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(insert_menu), page_break_item);

    GtkWidget *separator2 = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(insert_menu), separator2);

    GtkWidget *header_item = gtk_menu_item_new_with_label("Header");
    g_signal_connect(header_item, "activate", G_CALLBACK(on_insert_header_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(insert_menu), header_item);

    GtkWidget *footer_item = gtk_menu_item_new_with_label("Footer");
    g_signal_connect(footer_item, "activate", G_CALLBACK(on_insert_footer_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(insert_menu), footer_item);

    GtkWidget *separator3 = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(insert_menu), separator3);

    GtkWidget *link_item = gtk_menu_item_new_with_label("Hyperlink");
    g_signal_connect(link_item, "activate", G_CALLBACK(on_insert_link_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(insert_menu), link_item);

    GtkWidget *comment_item = gtk_menu_item_new_with_label("Comment");
    g_signal_connect(comment_item, "activate", G_CALLBACK(on_insert_comment_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(insert_menu), comment_item);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(insert_item), insert_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), insert_item);

    // ======================================
    // 5. FORMAT MENU
    // ======================================
    GtkWidget *format_item = gtk_menu_item_new_with_label("Format");
    GtkWidget *format_menu = gtk_menu_new();

    GtkWidget *bold_item = gtk_menu_item_new_with_label("Bold");
    g_signal_connect(bold_item, "activate", G_CALLBACK(on_format_bold_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), bold_item);

    GtkWidget *italic_item = gtk_menu_item_new_with_label("Italic");
    g_signal_connect(italic_item, "activate", G_CALLBACK(on_format_italic_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), italic_item);

    GtkWidget *underline_item = gtk_menu_item_new_with_label("Underline");
    g_signal_connect(underline_item, "activate", G_CALLBACK(on_format_underline_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), underline_item);

    GtkWidget *separator_fmt = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), separator_fmt);

    GtkWidget *align_left_item = gtk_menu_item_new_with_label("Align Left");
    g_signal_connect(align_left_item, "activate", G_CALLBACK(on_format_align_left_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), align_left_item);

    GtkWidget *align_center_item = gtk_menu_item_new_with_label("Align Center");
    g_signal_connect(align_center_item, "activate", G_CALLBACK(on_format_align_center_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), align_center_item);

    GtkWidget *align_right_item = gtk_menu_item_new_with_label("Align Right");
    g_signal_connect(align_right_item, "activate", G_CALLBACK(on_format_align_right_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), align_right_item);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(format_item), format_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), format_item);

    // ======================================
    // 6. TOOLS MENU 
    // ======================================
    GtkWidget *tools_item = gtk_menu_item_new_with_label("Tools");
    GtkWidget *tools_menu = gtk_menu_new();

    // Word Count (Compteur de mots)
    GtkWidget *word_count_item = gtk_menu_item_new();
    GtkWidget *wc_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *wc_image = gtk_image_new_from_icon_name("document-properties", GTK_ICON_SIZE_MENU);
    GtkWidget *wc_label = gtk_label_new("Word Count");
    gtk_box_pack_start(GTK_BOX(wc_box), wc_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(wc_box), wc_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(word_count_item), wc_box);
    g_signal_connect(word_count_item, "activate", G_CALLBACK(on_tools_word_count_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(tools_menu), word_count_item);

    // Hunspell Spellcheck (Vérification orthographique locale)
    GtkWidget *spellcheck_item = gtk_menu_item_new();
    GtkWidget *sc_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *sc_image = gtk_image_new_from_icon_name("spelling-check", GTK_ICON_SIZE_MENU);
    GtkWidget *sc_label = gtk_label_new("Spellcheck (Hunspell)");
    gtk_box_pack_start(GTK_BOX(sc_box), sc_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sc_box), sc_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(spellcheck_item), sc_box);
    g_signal_connect(spellcheck_item, "activate", G_CALLBACK(on_tools_spellcheck_clicked), app_widgets);
    gtk_widget_add_accelerator(spellcheck_item, "activate", accel_group, GDK_KEY_F7, 0, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(tools_menu), spellcheck_item);

    GtkWidget *separator_tools = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(tools_menu), separator_tools);

    // Preferences / Options
    GtkWidget *prefs_item = gtk_menu_item_new();
    GtkWidget *prefs_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *prefs_image = gtk_image_new_from_icon_name("preferences-system", GTK_ICON_SIZE_MENU);
    GtkWidget *prefs_label = gtk_label_new("Preferences");
    gtk_box_pack_start(GTK_BOX(prefs_box), prefs_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(prefs_box), prefs_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(prefs_item), prefs_box);
    g_signal_connect(prefs_item, "activate", G_CALLBACK(on_tools_preferences_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(tools_menu), prefs_item);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(tools_item), tools_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), tools_item);

    // ======================================
    // 7. TABLE MENU 
    // ======================================
    GtkWidget *Table_item = gtk_menu_item_new_with_label("Table");
    GtkWidget *Tablee_menu = gtk_menu_new();

    // Insert Row Above
    GtkWidget *row_above_item = gtk_menu_item_new_with_label("Insert Row Above");
    g_signal_connect(row_above_item, "activate", G_CALLBACK(on_table_insert_row_above_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(Tablee_menu), row_above_item);

    // Insert Row Below
    GtkWidget *row_below_item = gtk_menu_item_new_with_label("Insert Row Below");
    g_signal_connect(row_below_item, "activate", G_CALLBACK(on_table_insert_row_below_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(Tablee_menu), row_below_item);

    GtkWidget *separator_tbl1 = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(Tablee_menu), separator_tbl1);

    // Insert Column Left
    GtkWidget *col_left_item = gtk_menu_item_new_with_label("Insert Column Left");
    g_signal_connect(col_left_item, "activate", G_CALLBACK(on_table_insert_col_left_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(Tablee_menu), col_left_item);

    // Insert Column Right
    GtkWidget *col_right_item = gtk_menu_item_new_with_label("Insert Column Right");
    g_signal_connect(col_right_item, "activate", G_CALLBACK(on_table_insert_col_right_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(Tablee_menu), col_right_item);

    GtkWidget *separator_tbl2 = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(Tablee_menu), separator_tbl2);

    // Delete Table Elements
    GtkWidget *delete_row_item = gtk_menu_item_new_with_label("Delete Row");
    g_signal_connect(delete_row_item, "activate", G_CALLBACK(on_table_delete_row_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(Tablee_menu), delete_row_item);

    GtkWidget *delete_col_item = gtk_menu_item_new_with_label("Delete Column");
    g_signal_connect(delete_col_item, "activate", G_CALLBACK(on_table_delete_col_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(Tablee_menu), delete_col_item);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(Table_item), Tablee_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), Table_item);

    // ======================================
    // 8. WINDOW MENU 
    // ======================================
    GtkWidget *window_item = gtk_menu_item_new_with_label("Window");
    GtkWidget *window_menu = gtk_menu_new();

    // New Window (Ouvrir une nouvelle instance)
    GtkWidget *new_window_item = gtk_menu_item_new();
    GtkWidget *win_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *win_image = gtk_image_new_from_icon_name("window-new", GTK_ICON_SIZE_MENU);
    GtkWidget *win_label = gtk_label_new("New Window");
    gtk_box_pack_start(GTK_BOX(win_box), win_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(win_box), win_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(new_window_item), win_box);
    g_signal_connect(new_window_item, "activate", G_CALLBACK(on_window_new_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(window_menu), new_window_item);

    // Fullscreen Mode
    GtkWidget *fullscreen_item = gtk_menu_item_new();
    GtkWidget *fs_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *fs_image = gtk_image_new_from_icon_name("view-fullscreen", GTK_ICON_SIZE_MENU);
    GtkWidget *fs_label = gtk_label_new("Fullscreen");
    gtk_box_pack_start(GTK_BOX(fs_box), fs_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(fs_box), fs_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(fullscreen_item), fs_box);
    g_signal_connect(fullscreen_item, "activate", G_CALLBACK(on_window_fullscreen_clicked), app_widgets);
    gtk_widget_add_accelerator(fullscreen_item, "activate", accel_group, GDK_KEY_F11, 0, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(window_menu), fullscreen_item);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(window_item), window_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), window_item);

    // ======================================
    // 9. HELP MENU 
    // ======================================
    GtkWidget *help_item = gtk_menu_item_new_with_label("Help");
    GtkWidget *help_menu = gtk_menu_new();
    
    // Help Contents
    GtkWidget *contents_item = gtk_menu_item_new();
    GtkWidget *hc_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *hc_image = gtk_image_new_from_icon_name("help-browser", GTK_ICON_SIZE_MENU);
    GtkWidget *hc_label = gtk_label_new("Help Contents");
    gtk_box_pack_start(GTK_BOX(hc_box), hc_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hc_box), hc_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(contents_item), hc_box);
    g_signal_connect(contents_item, "activate", G_CALLBACK(on_help_contents_clicked), app_widgets);
    gtk_widget_add_accelerator(contents_item, "activate", accel_group, GDK_KEY_F1, 0, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), contents_item);

    // About Dialog (Boîte de dialogue À propos)
    GtkWidget *about_item = gtk_menu_item_new();
    GtkWidget *about_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *about_image = gtk_image_new_from_icon_name("help-about", GTK_ICON_SIZE_MENU);
    GtkWidget *about_label = gtk_label_new("About IntelliEditor");
    gtk_box_pack_start(GTK_BOX(about_box), about_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(about_box), about_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(about_item), about_box);
    g_signal_connect(about_item, "activate", G_CALLBACK(on_help_about_clicked), app_widgets);
    gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), about_item);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(help_item), help_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), help_item);

    return menu_bar;
}

/* =========================================================
   FORMAT TOOLBAR 2 (WORD-LIKE)
   ========================================================= */
GtkWidget* create_format_toolbar(AppWidgets *app_widgets)
{
    GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_set_name(toolbar, "format-toolbar");

    gtk_widget_set_margin_top(toolbar, 6);
    gtk_widget_set_margin_bottom(toolbar, 6);
    gtk_widget_set_margin_start(toolbar, 10);
    gtk_widget_set_margin_end(toolbar, 10);

    /* ================= BOLD ================= */
    GtkWidget *bold_btn = gtk_button_new_with_label("B");
    gtk_widget_set_name(bold_btn, "format-button");
    gtk_box_pack_start(GTK_BOX(toolbar), bold_btn, FALSE, FALSE, 0);
    g_signal_connect(bold_btn, "clicked", G_CALLBACK(on_format_bold_clicked), app_widgets);

    /* ================= ITALIC ================= */
    GtkWidget *italic_btn = gtk_button_new_with_label("I");
    gtk_widget_set_name(italic_btn, "format-button");
    gtk_box_pack_start(GTK_BOX(toolbar), italic_btn, FALSE, FALSE, 0);
    g_signal_connect(italic_btn, "clicked", G_CALLBACK(on_format_italic_clicked), app_widgets);

    /* ================= UNDERLINE ================= */
    GtkWidget *underline_btn = gtk_button_new_with_label("U");
    gtk_widget_set_name(underline_btn, "format-button");
    gtk_box_pack_start(GTK_BOX(toolbar), underline_btn, FALSE, FALSE, 0);
    g_signal_connect(underline_btn, "clicked", G_CALLBACK(on_format_underline_clicked), app_widgets);

    /* ================= FONT SIZE ================= */
    GtkWidget *size_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(size_combo), "10");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(size_combo), "12");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(size_combo), "14");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(size_combo), "16");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(size_combo), "18");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(size_combo), "24");
    gtk_combo_box_set_active(GTK_COMBO_BOX(size_combo), 2);
    gtk_box_pack_start(GTK_BOX(toolbar), size_combo, FALSE, FALSE, 0);
    g_signal_connect(size_combo, "changed", G_CALLBACK(on_format_font_size_changed), app_widgets);

    /* ================= FONT FAMILY ================= */
    GtkWidget *font_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(font_combo), "Calibri");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(font_combo), "Arial");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(font_combo), "Times New Roman");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(font_combo), "Sans");
    gtk_combo_box_set_active(GTK_COMBO_BOX(font_combo), 0);
    gtk_box_pack_start(GTK_BOX(toolbar), font_combo, FALSE, FALSE, 0);
    g_signal_connect(font_combo, "changed", G_CALLBACK(on_format_font_family_changed), app_widgets);

    /* ================= ALIGN LEFT ================= */
    GtkWidget *align_left = gtk_button_new_with_label("⬅");
    gtk_widget_set_name(align_left, "format-button");
    gtk_box_pack_start(GTK_BOX(toolbar), align_left, FALSE, FALSE, 0);
    g_signal_connect(align_left, "clicked", G_CALLBACK(on_format_align_left_clicked), app_widgets);

    /* ================= ALIGN CENTER ================= */
    GtkWidget *align_center = gtk_button_new_with_label("⬌");
    gtk_widget_set_name(align_center, "format-button");
    gtk_box_pack_start(GTK_BOX(toolbar), align_center, FALSE, FALSE, 0);
    g_signal_connect(align_center, "clicked", G_CALLBACK(on_format_align_center_clicked), app_widgets);

    /* ================= ALIGN RIGHT ================= */
    GtkWidget *align_right = gtk_button_new_with_label("➡");
    gtk_widget_set_name(align_right, "format-button");
    gtk_box_pack_start(GTK_BOX(toolbar), align_right, FALSE, FALSE, 0);
    g_signal_connect(align_right, "clicked", G_CALLBACK(on_format_align_right_clicked), app_widgets);

    return toolbar;
}