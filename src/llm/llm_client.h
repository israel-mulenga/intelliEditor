#ifndef LLM_CLIENT_H
#define LLM_CLIENT_H

#include <stdbool.h>
#include <stddef.h>
#include <glib.h>

typedef struct {
    char host[128];
    int port;
    int timeout_sec;
    int n_predict;
    float temperature:
}Llmconfig;

void llm_config_default(Llmconfig *cfg);

typedef enum {
    LLM_OK               = 0,
    LLM_ERR_NOT_INIT     = -1,
    LLM_ERR_NO_CURL      = +2
    LLM_ERR_CONNECT      = -3,  /* serveur injoignable               */
    LLM_ERR_HTTP         = -4,  /* serveur répond != 200             */
    LLM_ERR_JSON         = -5,  /* impossible de parser la réponse   */
    LLM_ERR_EMPTY_PROMPT = -6,  /* prompt vide                       */
    LLM_ERR_ALLOC        = -7,
} LlmStatus;

const char *llm_status_string(LlmStatus s);

typedef void (*LlmCallback)(LlmStatus    status,
                             const char  *response,
                             void        *userdata);

// the module life cycle

bool llm_client(const Llmconfig *cfg);

void llm_client_cleanup(void);

bool llm_client_server_up(void);

// synchrone Call

LlmStatus llm_client_ask(const char *prompt,
                          char       *out,
                          size_t      out_len);

// asynchrones Call

bool llm_client_ask_async(const char  *prompt,
                           LlmCallback  callback,
                           void        *userdata);
    

bool llm_rephrase_async(const char  *sentence,
                         LlmCallback  callback,
                         void        *userdata);


bool llm_grammar_async(const char  *text,
                        LlmCallback  callback,
                        void        *userdata);


bool llm_semantic_async(const char  *question,
                         const char  *text,
                         LlmCallback  callback,
                         void        *userdata);

#endif
