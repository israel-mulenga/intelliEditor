# Instructions pour DEV-B — Intégration LLM

## Ce qui est prêt côté DEV-C

Les fichiers suivants sont disponibles dans `src/llm/` :
- `llm_client.h/c` — client HTTP vers llama-server
- `llm_bridge.h/c` — pont entre LLM et UI GTK
- les fihiers.h sont dans include.

## Ce que tu dois faire

### 1. Inclure le bridge

Dans ton fichier de toolbar ou de callbacks UI : C'EST FAIT

```c
#include "llm/llm_bridge.h"
```

### 2. Remplir AppContext : C'EST FAIT

```c
// Dans ton init GTK :
AppContext *app = g_new(AppContext, 1);
app->source_view = ton_source_view;
app->statusbar   = ta_statusbar;
app->sidebar     = ta_sidebar;  // NULL si pas encore prête
```

    ### 3. Connecter les boutons : C'EST FAIT

    ```c
    // Bouton "Reformuler"
    g_signal_connect(btn_rewrite, "clicked",
                    G_CALLBACK(llm_bridge_on_rewrite_clicked),
                    app);

    // Bouton "Corriger la grammaire"
    g_signal_connect(btn_grammar, "clicked",
                    G_CALLBACK(llm_bridge_on_grammar_clicked),
                    app);
    ```

### 4. Initialiser le LLM au démarrage : C'EST FAIT


```c
// Dans ta fonction d'init, AVANT d'afficher la fenêtre :
llm_client_init(NULL);  // NULL = valeurs par défaut (port 8080)

// Optionnel : désactiver les boutons si serveur absent
if (!llm_client_server_up()) {
    gtk_widget_set_sensitive(btn_rewrite, FALSE);
    gtk_widget_set_sensitive(btn_grammar, FALSE);
    gtk_statusbar_push(GTK_STATUSBAR(statusbar), 0,
        "⚠ LLM non disponible — lancer start_llm.sh");
}




MOI(DEV B) J'AI FAIT

/* Initialiser le client LLM (facultatif) */
    if (llm_client_init(NULL)) {
        if (!llm_client_server_up()) {
            if (app_widgets->statusbar) {
                gtk_statusbar_push(GTK_STATUSBAR(app_widgets->statusbar), 0,
                    "⚠ LLM non disponible (serveur injoignable). Veuillez lancer start_llm.sh");
            }
        }
    } else {
        if (app_widgets->statusbar) {
            gtk_statusbar_push(GTK_STATUSBAR(app_widgets->statusbar), 0,

                "⚠ LLM initialisation échouée");
        }
    }
```

### 5. Libérer à la fermeture : PAS ENCORE FAIT

```c
// Dans ton callback de fermeture de fenêtre :
llm_client_cleanup();
```

## Serveur LLM : PAS ENCORE FAIT

Le serveur doit être lancé avant l'application :
```bash
~/llama.cpp/build/bin/llama-server \
  -m "/home/alfred/Documents/Projects/C Project/finale project/models/Llama-3.2-3B-Instruct-Q4_K_M.gguf" \
  --port 8080 -c 2048
```
