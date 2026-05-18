#ifndef WINDOW_H
#define WINDOW_H

#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include <hunspell/hunspell.h>
#include "config/config_parser.h"
#include "editor/gap_buffer.h"
#include "rules/rules.h"

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
    GtkWidget *horizontal_ruler;
    GtkWidget *vertical_ruler;
    GtkWidget *notebook;
    GapBuffer *gb;

    GtkSourceBuffer *editor_buffer;
    gulong insert_handler_id;
    gulong delete_handler_id;
    gchar *current_file_path;

    AppConfig config;
    guint autosave_source_id;
    guint autosave_interval_sec;

    Hunhandle *hun_en;        // Dictionnaire anglais
    Hunhandle *hun_fr;        // Dictionnaire francais

    RuleSet *ruleset;
    gchar *rules_file_path;

} AppWidgets;

/* =========================================================
   FONCTIONS PRINCIPALES UI
   ========================================================= */
void create_main_window(GtkApplication *app, gpointer user_data);

void app_autosave_reschedule(AppWidgets *app);

GtkWidget *create_toolbar(AppWidgets *app, GtkAccelGroup *accel_group);
GtkWidget *create_editor(void);
GtkWidget *create_sidebar(void);
GtkWidget* create_new_tab(AppWidgets *app_widgets, const gchar *title);
void append_new_page_to_current_document(AppWidgets *app_widgets);

/* =========================================================
   CALLBACKS (actions boutons)
   ========================================================= */
void on_correct_clicked(GtkWidget *widget, gpointer data);
void on_rewrite_clicked(GtkWidget *widget, gpointer data);
void setup_application_shortcuts(AppWidgets *app);

#endif