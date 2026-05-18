#include "ui/sidebar.h"

void sidebar_set_rules_file(GtkWidget *sidebar, const char *filename) {
    if (!sidebar) {
        return;
    }

    GtkWidget *label = GTK_WIDGET(g_object_get_data(G_OBJECT(sidebar), "rules-file-label"));
    if (!GTK_IS_LABEL(label)) {
        return;
    }

    if (filename && *filename) {
        gchar *markup = g_strdup_printf("Fichier : <b>%s</b>", filename);
        gtk_label_set_markup(GTK_LABEL(label), markup);
        g_free(markup);
    } else {
        gtk_label_set_text(GTK_LABEL(label), "Fichier : (aucun)");
    }
}

GtkWidget *create_sidebar(void) {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_name(vbox, "sidebar");
    gtk_widget_set_size_request(vbox, 280, -1);

    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<b>Panneau des règles</b>");
    gtk_label_set_xalign(GTK_LABEL(title), 0.0);
    gtk_widget_set_margin_start(title, 12);
    gtk_widget_set_margin_end(title, 12);
    gtk_widget_set_margin_top(title, 12);
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);

    GtkWidget *file_label = gtk_label_new("Fichier : (aucun)");
    gtk_label_set_xalign(GTK_LABEL(file_label), 0.0);
    gtk_widget_set_margin_start(file_label, 12);
    gtk_widget_set_margin_end(file_label, 12);
    g_object_set_data(G_OBJECT(vbox), "rules-file-label", file_label);
    gtk_box_pack_start(GTK_BOX(vbox), file_label, FALSE, FALSE, 0);

    if (g_file_test("config.json", G_FILE_TEST_EXISTS)) {
        sidebar_set_rules_file(vbox, "config.json");
    }

    GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), separator, FALSE, FALSE, 4);

    GtkWidget *summary = gtk_label_new(
        "Conformité : en attente d'analyse.\n"
        "Utilisez « Correct » ou chargez des règles JSON."
    );
    gtk_label_set_xalign(GTK_LABEL(summary), 0.0);
    gtk_label_set_line_wrap(GTK_LABEL(summary), TRUE);
    gtk_label_set_line_wrap_mode(GTK_LABEL(summary), PANGO_WRAP_WORD);
    gtk_widget_set_margin_start(summary, 12);
    gtk_widget_set_margin_end(summary, 12);
    g_object_set_data(G_OBJECT(vbox), "sidebar-label", summary);
    gtk_box_pack_start(GTK_BOX(vbox), summary, FALSE, FALSE, 0);

    GtkWidget *list_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(list_title), "<b>Détail</b>");
    gtk_label_set_xalign(GTK_LABEL(list_title), 0.0);
    gtk_widget_set_margin_start(list_title, 12);
    gtk_widget_set_margin_top(list_title, 8);
    gtk_box_pack_start(GTK_BOX(vbox), list_title, FALSE, FALSE, 0);

    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(
        GTK_SCROLLED_WINDOW(scrolled),
        GTK_POLICY_NEVER,
        GTK_POLICY_AUTOMATIC
    );
    gtk_widget_set_margin_start(scrolled, 8);
    gtk_widget_set_margin_end(scrolled, 8);
    gtk_widget_set_margin_bottom(scrolled, 12);

    GtkWidget *listbox = gtk_list_box_new();
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(listbox), GTK_SELECTION_NONE);
    gtk_container_add(GTK_CONTAINER(scrolled), listbox);
    g_object_set_data(G_OBJECT(vbox), "error-list", listbox);

    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);

    return vbox;
}

void sidebar_set_summary(GtkWidget *sidebar, const char *text) {
    if (!sidebar || !text) {
        return;
    }

    GtkWidget *label = GTK_WIDGET(g_object_get_data(G_OBJECT(sidebar), "sidebar-label"));
    if (GTK_IS_LABEL(label)) {
        gtk_label_set_text(GTK_LABEL(label), text);
    }
}

GtkListBox *sidebar_get_rules_list(GtkWidget *sidebar) {
    if (!sidebar) {
        return NULL;
    }

    return GTK_LIST_BOX(g_object_get_data(G_OBJECT(sidebar), "error-list"));
}
