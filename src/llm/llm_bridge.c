#include "llm/llm_bridge.h"
#include "llm/llm_client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    GtkSourceView *view;
    char          *new_text;

} ReplaceData;

typedef struct{
    GtkWidget *statusbar;
    char      *message;
} StatusData;


static gboolean idle_replace_selection(gpointer data){
    ReplaceData *d = (ReplaceData *)data;

    GtkTextBuffer *buffer =
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(d->view));
    
    
    GtkTextIter start, end;
    gboolean has_selection =
        gtk_text_buffer_get_selection_bounds(buffer, &start, &end);

    if(has_selection){
        
        gtk_text_buffer_begin_user_action(buffer);
        gtk_text_buffer_delete(buffer, &start, &end);
        gtk_text_buffer_insert(buffer, &start, d->new_text, -1);
        gtk_text_buffer_end_user_action(buffer);

    } else{
        gtk_text_buffer_insert_at_cursor(buffer, d->new_text, -1);
    }

    g_free(d->new_text);
    g_free(d);

    return G_SOURCE_REMOVE;
}

typedef struct{
    GtkSourceView *view;
    GtkWidget     *statusbar;
    GtkWidget     *sidebar;
} CallbackContext;

static gboolean idle_update_status(gpointer data){
    StatusData *sd = (StatusData *)data;
    if (sd->statusbar && sd->message){
        guint context_id = gtk_statusbar_get_context_id(
            GTK_STATUSBAR(sd->statusbar), "llm"
        );
        gtk_statusbar_push(GTK_STATUSBAR(sd->statusbar), context_id, sd->message);
    }
    g_free(sd->message);
    g_free(sd);
    return G_SOURCE_REMOVE;
}

static gboolean idle_show_status(gpointer data){
    return idle_update_status(data);
}

// reformulate the prompt to ask for a rephrasing

static void on_rephrase_result(LlmStatus status,
                                const char *response,
                                void      *userdata){
    CallbackContext *ctx = (CallbackContext *)userdata;

    if (status == LLM_OK && response && response[0] != '\0') {
        StatusData *sd = g_new(StatusData, 1);
        sd->statusbar = ctx->statusbar;
        sd->message = g_strdup("Reformulation réussie !");
        g_idle_add(idle_show_status, sd);
    } else {
        const char *msg;
        switch (status){
            case LLM_ERR_CONNECT:
                msg = "Erreur de connexion au serveur LLM.";
                break;
            case LLM_ERR_JSON:
                msg = "Erreur de parsing de la réponse du serveur LLM.";
                break;
            default:
                msg = "Erreur inconnue lors de la reformulation.";
                break;
        }

        StatusData *sd = g_new(StatusData, 1);
        sd->statusbar = ctx->statusbar;
        sd->message = g_strdup(msg);
        g_idle_add(idle_show_status, sd);
    }

    g_free(ctx);
}

static void on_grammar_result(LlmStatus status,
                                const char *response,
                                void      *userdata){
    CallbackContext *ctx = (CallbackContext *)userdata;

    StatusData *sd = g_new(StatusData, 1);
    sd->statusbar = ctx->statusbar;

    if (status == LLM_OK && response){
        if (strstr(response, "\"errors\":[]") != NULL){
            sd->message = g_strdup("Aucune erreur grammaticale détectée !");
        } else {
            sd->message = g_strdup("Des erreurs grammaticales ont été détectées.");
        }
    } else {
        sd->message = g_strdup("LLM: vérifications grammaticales échouées.");
    }

    g_idle_add(idle_update_status, sd);
    g_free(ctx);
}

char *llm_bridge_get_selected_text(GtkSourceView *view){
    if (!view) return NULL;

    GtkTextBuffer *buffer =
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));

    GtkTextIter start, end;
    if (gtk_text_buffer_get_selection_bounds(buffer, &start, &end)){
        return gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    }

    GtkTextIter cursor;
    gtk_text_buffer_get_iter_at_mark(
        buffer, &cursor, gtk_text_buffer_get_insert(buffer)
    );

    GtkTextIter sentence_start = cursor;
    while (!gtk_text_iter_is_start(&sentence_start)){
        gtk_text_iter_backward_char(&sentence_start);
        gunichar c = gtk_text_iter_get_char(&sentence_start);
        if (c == '.' || c == '!' || c == '?'){
            gtk_text_iter_forward_char(&sentence_start);
            break;
        }
    }

    GtkTextIter sentence_end = cursor;
    while (!gtk_text_iter_is_end(&sentence_end)){
        gunichar c = gtk_text_iter_get_char(&sentence_end);
        gtk_text_iter_forward_char(&sentence_end);
        if (c == '.' || c == '!' || c == '?'){
            break;
        }
    }

    char *text = gtk_text_buffer_get_text(
        buffer, &sentence_start, &sentence_end, FALSE
    );
    if (text && strlen(text) < 3){
        g_free(text);
        return NULL;
    }

    return text;
}

void llm_bridge_replace_selection(GtkSourceView *view,
                                const char *new_text){
    if (!view || !new_text) return;
    ReplaceData *d = g_new(ReplaceData, 1);
    d->view = view;
    d->new_text = g_strdup(new_text);

    g_idle_add(idle_replace_selection, d);
}

void llm_bridge_show_status(GtkWidget *statusbar,
                            const char *message){
    if (!statusbar || !message) return;

    StatusData *sd = g_new(StatusData, 1);
    sd->statusbar = statusbar;
    sd->message = g_strdup(message);

    g_idle_add(idle_update_status, sd);
}

void llm_bridge_on_write_clicked(GtkWidget *widget, gpointer user_data){
    (void)widget;

    AppContext *ctx = (AppContext *)user_data;
    if (!ctx || !ctx->source_view) {
        fprintf(stderr, "[Bridge] AppContext invalide dans on_write_clicked\n");
        return;
    }

    char *text = llm_bridge_get_selected_text(ctx->source_view);
    if (!text){
        llm_bridge_show_status(ctx->statusbar,
            "Aucun texte sélectionné pour la reformulation.");
        return;
    }

    llm_bridge_show_status(ctx->statusbar,
         "En cours de reformulation...");

    CallbackContext *cb_ctx = g_new(CallbackContext, 1);
    cb_ctx->view = ctx->source_view;
    cb_ctx->statusbar = ctx->statusbar;
    cb_ctx->sidebar = ctx->sidebar;

    bool ok = llm_rephrase_async(text, on_rephrase_result, cb_ctx);
    if (!ok){
        llm_bridge_show_status(ctx->statusbar,
            "Erreur lors de l'envoi de la requête de reformulation.");
        g_free(cb_ctx);
    }

    g_free(text);
}

void llm_bridge_on_gramma_clicked(GtkWidget *widget, gpointer user_data){
    (void)widget;

    AppContext *app = (AppContext *)user_data;
    if (!app || !app->source_view) return;

    GtkTextBuffer *buffer =
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->source_view));

    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char *full_text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    if (!full_text || strlen(full_text) < 5){
        llm_bridge_show_status(app->statusbar,
            "Le document est vide pour la vérification grammaticale.");
        g_free(full_text);
        return;
    }

    CallbackContext *ctx = g_new(CallbackContext, 1);
    ctx->view = app->source_view;
    ctx->statusbar = app->statusbar;
    ctx->sidebar = app->sidebar;

    bool ok = llm_grammar_async(full_text, on_grammar_result, ctx);
    if (!ok){
        llm_bridge_show_status(app->statusbar,
            "Erreur lors de l'envoi de la requête de vérification grammaticale.");
        g_free(ctx);
    }
    g_free(full_text);
}

