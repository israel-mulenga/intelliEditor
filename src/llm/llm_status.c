/**
 * llm_status.c
 * Error messages and llama-server status monitoring.
 */

#define _GNU_SOURCE

#include "llm/llm_status.h"
#include "llm/llm_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* ERROR MESSAGES */


const char *llm_error_message(LlmStatus status) {
    switch (status) {
        case LLM_OK:
            return "✓ LLM: operation succeeded";
        case LLM_ERR_NOT_INIT:
            return "⚠ LLM not initialized (llm_client_init missing)";
        case LLM_ERR_CURL:
            return "⚠ libcurl not available";
        case LLM_ERR_CONNECT:
            return "⚠ LLM unavailable — run: ./start_llm.sh";
        case LLM_ERR_HTTP:
            return "⚠ LLM server error — restart llama-server";
        case LLM_ERR_JSON:
            return "⚠ LLM response unreadable — context too long?";
        case LLM_ERR_EMPTY:
            return "⚠ Select text before using the LLM";
        case LLM_ERR_ALLOC:
            return "⚠ Insufficient memory";
        default:
            return "⚠ Unknown LLM error";
    }
}


/* SERVER MONITORING */


static guint  g_watch_id   = 0;
static bool   g_last_state = false;

typedef struct {
    LlmServerCallback callback;
    void             *userdata;
} WatchData;

static WatchData *g_watch_data = NULL;

static gboolean check_server(gpointer data) {
    WatchData *wd = (WatchData *)data;
    bool current  = llm_client_server_up();

    if (current != g_last_state) {
        g_last_state = current;
        fprintf(stdout, "[LLM Status] Server %s\n",
                current ? "available ✓" : "unavailable ✗");
        if (wd->callback)
            wd->callback(current, wd->userdata);
    }

    return G_SOURCE_CONTINUE;
}

void llm_status_watch_start(LlmServerCallback callback, void *userdata) {
    llm_status_watch_stop();

    g_watch_data = g_new(WatchData, 1);
    g_watch_data->callback = callback;
    g_watch_data->userdata = userdata;

    g_last_state = llm_client_server_up();
    if (callback) callback(g_last_state, userdata);

    g_watch_id = g_timeout_add(5000, check_server, g_watch_data);
}

void llm_status_watch_stop(void) {
    if (g_watch_id != 0) {
        g_source_remove(g_watch_id);
        g_watch_id = 0;
    }
    if (g_watch_data) {
        g_free(g_watch_data);
        g_watch_data = NULL;
    }
}


/* BUTTON UPDATE (thread-safe) */

typedef struct {
    GtkWidget *btn_rewrite;
    GtkWidget *btn_grammar;
    bool       is_available;
} ButtonData;

static gboolean idle_update_buttons(gpointer data) {
    ButtonData *d = (ButtonData *)data;
    if (d->btn_rewrite)
        gtk_widget_set_sensitive(d->btn_rewrite, d->is_available);
    if (d->btn_grammar)
        gtk_widget_set_sensitive(d->btn_grammar, d->is_available);
    g_free(d);
    return G_SOURCE_REMOVE;
}

void llm_status_update_buttons(GtkWidget *btn_rewrite,
                                GtkWidget *btn_grammar,
                                bool       is_available) {
    ButtonData *d = g_new(ButtonData, 1);
    d->btn_rewrite  = btn_rewrite;
    d->btn_grammar  = btn_grammar;
    d->is_available = is_available;
    g_idle_add(idle_update_buttons, d);
}