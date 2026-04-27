#ifndef WINDOW_H
#define WINDOW_H

#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include <hunspell/hunspell.h>
#include "editor/gap_buffer.h"

/* =========================================================
   STRUCTURE GLOBALE
   Contient toutes les references partagees de l'application
   ========================================================= */
typedef struct {

    GtkApplication *app;      // Application GTK principale

    GtkWidget *window;        // Fenetre principale
    GtkWidget *stack;         // Gestion des pages (welcome / editor)
    GtkWidget *sidebar;       // Panneau lateral
    GtkWidget *statusbar;     // Barre de statut
    GtkWidget *editor_view;   // Vue d'edition
    GapBuffer *gb;

    GtkSourceBuffer *editor_buffer;
    gulong insert_handler_id;
    gulong delete_handler_id;
    gchar *current_file_path;

    Hunhandle *hun_en;        // Dictionnaire anglais
    Hunhandle *hun_fr;        // Dictionnaire francais

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