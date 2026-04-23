#ifndef WINDOW_H
#define WINDOW_H

#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include <hunspell/hunspell.h>

/* =========================================================
   STRUCTURE GLOBALE
   Contient toutes les références partagées de l'application
   ========================================================= */
typedef struct {

    GtkApplication *app;      // Application GTK principale

    GtkWidget *stack;         // Gestion des pages (welcome / editor)
    GtkWidget *sidebar;       // Panneau latéral
    GtkWidget *statusbar;     // Barre de statut

    GtkSourceBuffer *editor_buffer;

    Hunhandle *hun_en;        // dictionnaire anglais
    Hunhandle *hun_fr;        // dictionnaire français

} AppWidgets;

/* =========================================================
   FONCTIONS PRINCIPALES UI
   ========================================================= */
void create_main_window(GtkApplication *app, gpointer user_data);

GtkWidget *create_toolbar(AppWidgets *app);
GtkWidget *create_editor(void);
GtkWidget *create_sidebar(void);

/* =========================================================
   CALLBACKS (actions boutons)
   ========================================================= */
void on_correct_clicked(GtkWidget *widget, gpointer data);
void on_rewrite_clicked(GtkWidget *widget, gpointer data);

#endif