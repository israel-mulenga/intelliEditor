#include <gtk/gtk.h>
#include "ui/editor.h"
#include "../../include/ui/window.h"

// ======================================
// MODULE: SIDEBAR
// ROLE: Display rules and analysis results
// ======================================

GtkWidget* create_sidebar() {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_name(vbox, "sidebar");
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *listbox = gtk_list_box_new();

    gtk_widget_set_size_request(vbox, 200, -1); // Largeur fixe
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

    gtk_container_add(GTK_CONTAINER(scrolled), listbox);

    GtkWidget *label = gtk_label_new("Aucun résultat pour l'instant.");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0);
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 8);
    g_object_set_data(G_OBJECT(vbox), "sidebar-label", label);

    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);

    // Enregistrez le listbox pour y ajouter des lignes dynamiquement plus tard
    g_object_set_data(G_OBJECT(vbox), "error-list", listbox);
    
    return vbox;
}
