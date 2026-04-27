#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "../src/llm/llm_client.h"


/* Couleurs pour l'affichage */
#define GRN "\033[32m"
#define RED "\033[31m"
#define YLW "\033[33m"
#define RST "\033[0m"
#define BLD "\033[1m"

/* TEST 1 — Initialisation                                          */


static void test_init(void) {
    printf(BLD "\n  TEST 1 : Initialisation   " RST "\n");

    bool ok = llm_client_init(NULL); /* NULL = valeurs par défaut */

    if (ok) {
        printf(GRN "   llm_client_init() réussi" RST "\n");
    } else {
        printf(RED "   llm_client_init() échoué" RST "\n");
    }
}

/* TEST 2 — Serveur disponible                                      */


static void test_server(void) {
    printf(BLD "\n    TEST 2 : Serveur llama-server    " RST "\n");

    bool up = llm_client_server_up();

    if (up) {
        printf(GRN "   llama-server répond sur le port 8080" RST "\n");
    } else {
        printf(RED "   llama-server ne répond pas" RST "\n");
        printf(YLW "   Lancer le serveur dans un autre terminal :" RST "\n");
        printf("    ~/llama.cpp/build/bin/llama-server \\\n");
        printf("      -m \"/home/alfred/Documents/Projects/C Project/finale project/models/Llama-3.2-3B-Instruct-Q4_K_M.gguf\" \\\n");
        printf("      --port 8080 -c 2048\n");
    }
}

/* TEST 3 — Requête simple synchrone                                */


static void test_ask_sync(void) {
    printf(BLD "\n   TEST 3 : Requête synchrone   " RST "\n");

    if (!llm_client_server_up()) {
        printf(YLW "  ~ Ignoré (serveur absent)" RST "\n");
        return;
    }

    char reponse[1024] = {0};

    printf("  Envoi : \"corrige cette phrase : la sanctification ne pas l'absence du peche  ?\"\n");

    LlmStatus st = llm_client_ask(
        "<|begin_of_text|>"
        "<|start_header_id|>system<|end_header_id|>\n\n"
        "Tu es un assistant. Réponds en français en une seule phrase courte."
        "<|eot_id|>"
        "<|start_header_id|>user<|end_header_id|>\n\n"
        "corrige cette phrase : la sanctification ne pas l'absence du peche  ?"
        "<|eot_id|>"
        "<|start_header_id|>assistant<|end_header_id|>\n\n",
        reponse,
        sizeof(reponse)
    );

    if (st == LLM_OK) {
        printf(GRN "  ✓ Réponse reçue :" RST "\n");
        printf("    %s\n", reponse);
    } else {
        printf(RED "  ✗ Erreur : %s" RST "\n", llm_status_string(st));
    }
}

/* TEST 4 — Reformulation asynchrone                               */
typedef struct {
    GMutex mutex;
    GCond  cond;
    bool   done;
    char   resultat[2048];
    bool   succes;
} WaitResult;

static void callback_reformulation(LlmStatus   status,
                                    const char *reponse,
                                    void       *userdata) {
    WaitResult *w = (WaitResult *)userdata;


    g_mutex_lock(&w->mutex);
    w->done   = true;
    w->succes = (status == LLM_OK && reponse != NULL);
    if (reponse) {
        strncpy(w->resultat, reponse, sizeof(w->resultat) - 1);
    }
    g_cond_signal(&w->cond); 
    g_mutex_unlock(&w->mutex);
}

static void test_rephrase_async(void) {
    printf(BLD "\n=== TEST 4 : Reformulation asynchrone ===" RST "\n");

    if (!llm_client_server_up()) {
        printf(YLW "  ~ Ignoré (serveur absent)" RST "\n");
        return;
    }

    WaitResult w;
    memset(&w, 0, sizeof(w));
    g_mutex_init(&w.mutex);
    g_cond_init(&w.cond);

    const char *phrase = "les résultats de notre étude sont bons.";
    printf("  Phrase originale : \"%s\"\n", phrase);
    printf("  Envoi de la requête (non-bloquant)...\n");

    bool envoye = llm_rephrase_async(phrase, callback_reformulation, &w);

    if (!envoye) {
        printf(RED "  Impossible d'envoyer la requête" RST "\n");
        goto cleanup;
    }

    printf("  Attente de la réponse (max 60 secondes)...\n");

    g_mutex_lock(&w.mutex);
    gint64 limite = g_get_monotonic_time() + 60 * G_TIME_SPAN_SECOND;
    while (!w.done) {
        if (!g_cond_wait_until(&w.cond, &w.mutex, limite)) {
            printf(YLW "  ~ Timeout (60s)" RST "\n");
            break;
        }
    }
    g_mutex_unlock(&w.mutex);

    if (w.succes) {
        printf(GRN "   Reformulation reçue :" RST "\n");
        printf("    %s\n", w.resultat);
    } else {
        printf(RED "   Reformulation échouée" RST "\n");
    }

cleanup:
    g_mutex_clear(&w.mutex);
    g_cond_clear(&w.cond);
}

/* PROGRAMME PRINCIPAL                                              */

int main(void) {
    printf(BLD
    "\n╔════════════════════════════════════════╗\n"
      "║  IntelliEditor — Test LLM              ║\n"
      "║  Llama-3.2-3B-Instruct-Q4_K_M         ║\n"
      "╚════════════════════════════════════════╝\n"
    RST);

    test_init();
    test_server();
    test_ask_sync();
    test_rephrase_async();

    llm_client_cleanup();

    printf(BLD "\n════════════════════════════════════════\n" RST "\n");
    return 0;
}