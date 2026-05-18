/**
 * llm_status.h
 * Error handling and llama-server status monitoring.
 */

#ifndef LLM_STATUS_H
#define LLM_STATUS_H

#include <stdbool.h>
#include <gtk/gtk.h>
#include "llm_client.h"

/**
 * llm_error_message() — readable error message for the user.
 * Returns a static string — do not free.
 */
const char *llm_error_message(LlmStatus status);

/**
 * LlmServerCallback — called when the server status changes.
 * @param is_up    true = server has started
 *                 false = server has stopped
 */
typedef void (*LlmServerCallback)(bool is_up, void *userdata);

/**
 * llm_status_watch_start() — watches llama-server every 5 seconds.
 * Calls the callback only when the status changes.
 */
void llm_status_watch_start(LlmServerCallback callback, void *userdata);

/** llm_status_watch_stop() — stops status monitoring. */
void llm_status_watch_stop(void);

/**
 * llm_status_update_buttons() — enables/disables LLM buttons.
 * Thread-safe via g_idle_add().
 */
void llm_status_update_buttons(GtkWidget *btn_rewrite,
                                GtkWidget *btn_grammar,
                                bool       is_available);

#endif /* LLM_STATUS_H */