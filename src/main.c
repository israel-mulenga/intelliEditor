#include <gtk/gtk.h>
#include "ui/window.h"

// Cette fonction intermédiaire sert de pont pour correspondre aux types attendus
static void on_app_activate(GtkApplication *app, gpointer user_data) {
    create_main_window(app, user_data);
}

int main(int argc, char *argv[]) {
    // 1. Création de l'application GTK avec un identifiant unique
    GtkApplication *app = gtk_application_new("com.david.intellieditor", G_APPLICATION_DEFAULT_FLAGS);
    
    // 2. Connexion du signal d'activation à notre fonction qui crée la fenêtre
    g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), NULL);
    
    // 3. Exécution de l'application (gère automatiquement la boucle gtk_main interne)
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    
    // 4. Nettoyage de l'objet application
    g_object_unref(app);
    
    return status;
}