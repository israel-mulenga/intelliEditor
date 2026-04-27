#include "llm/llm_client.h"
#include "llm/llm_prompts.h"
#include "../../deps/cjson/cJSON.h"

#include <curl/curl.h>  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static LlmConfig g_cfg;
static bool g_initialized = false;
static char g_url_ask[245];
static char g_url_health[256];

typedef struct {
    char *data;
    size_t size;
} ResponseBuffer;

static size_t write_cb(void   *ptr,
                        size_t  size,
                        size_t  nmemb,
                        void   *userdata) {
    size_t  n   = size * nmemb; 
    ResponseBuffer *buf = (ResponseBuffer *)userdata;

 char *tmp = realloc(buf->data, buf->size + n + 1);
    if (!tmp) {
        fprintf(stderr, "[LLM] Mémoire insuffisante dans write_cb\n");
        return 0;  
    }

    buf->data = tmp;


    memcpy(buf->data + buf->size, ptr, n);
    buf->size            += n;
    buf->data[buf->size]  = '\0';   

    return n; 
}

void llm_config_default(LlmConfig *cfg){
    if(!cfg) return;
    strncpy(cfg->host, "127.0.0.1", sizeof(cfg->host) - 1);
    cfg->host[sizeof(cfg->host) - 1] = '\0';
    cfg->port       = 8080;
    cfg->timeout_sec = 60;
    cfg->n_predict = 256;
    cfg->temperature = 0.3f;
}

bool llm_client_init(const LlmConfig *cfg){
    if (cfg) {
        g_cfg = *cfg;
    }else {
        llm_config_default(&g_cfg);
    }

    snprintf(g_url_ask, sizeof(g_url_ask),
                "http://%s:%d/completion", g_cfg.host, g_cfg.port);
    snprintf(g_url_health, sizeof(g_url_health),
                "http://%s:%d/health",  g_cfg.host, g_cfg.port);
    
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
        fprintf(stderr, "[LLM] curl_global_init() a echouer\n");
        return false;
    }

    g_initialized =  true;
    fprintf(stdout, "[LLM] Client initialise -> %s\n", g_url_ask);
    return true;
}

void llm_client_cleanup(void) {
    if (!g_initialized) return;
    curl_global_cleanup();
    g_initialized = false;
    fprintf(stdout, "[LLM] client liberer.\n");
}

bool llm_client_server_up(void) {
    if (!g_initialized) return false;
    CURL *curl = curl_easy_init();
    if (!curl) return false;

    curl_easy_setopt(curl, CURLOPT_URL,           g_url_health);
    curl_easy_setopt(curl, CURLOPT_NOBODY,         1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT,        3L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 2L);

    CURLcode res = curl_easy_perform(curl);
    long     http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);

    return (res == CURLE_OK && http_code == 200);
}

static char *build_json(const char *prompt) {
    cJSON *root = cJSON_CreateObject();
    if (!root) return NULL;

    cJSON_AddStringToObject(root, "prompt",      prompt);
    cJSON_AddNumberToObject(root, "n_predict",   g_cfg.n_predict);
    cJSON_AddNumberToObject(root, "temperature", (double)g_cfg.temperature);


    cJSON *stop_arr = cJSON_AddArrayToObject(root, "stop");
    if (stop_arr) {
        cJSON_AddItemToArray(stop_arr, cJSON_CreateString(LLM_STOP_1));
        cJSON_AddItemToArray(stop_arr, cJSON_CreateString(LLM_STOP_2));
    }

    char *json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return json_str;    
}


static bool extract_content(const char *json_str,
                              char       *out_buf,
                              size_t      out_len) {
    if (!json_str || !out_buf) return false;

    cJSON *root = cJSON_Parse(json_str);
    if (!root) {
        fprintf(stderr, "[LLM] JSON invalide reçu du serveur.\n");
        fprintf(stderr, "[LLM] Début : %.100s\n", json_str);
        return false;
    }

    cJSON *content = cJSON_GetObjectItemCaseSensitive(root, "content");

    if (!cJSON_IsString(content) || !content->valuestring) {
        fprintf(stderr, "[LLM] Champ 'content' absent de la réponse.\n");
        cJSON_Delete(root);
        return false;
    }

    strncpy(out_buf, content->valuestring, out_len - 1);
    out_buf[out_len - 1] = '\0';

    cJSON_Delete(root);
    return true;
}

LlmStatus llm_client_ask(const char *prompt,
                          char       *out_buf,
                          size_t      out_len) {

    /* Verifications */
    if (!g_initialized)           return LLM_ERR_NOT_INIT;
    if (!prompt || !prompt[0])    return LLM_ERR_EMPTY;
    if (!out_buf || out_len == 0) return LLM_ERR_ALLOC;

    out_buf[0] = '\0';

    /* the resquest construction */
    char *req_json = build_json(prompt);
    if (!req_json) return LLM_ERR_ALLOC;

    /* the answer buffer*/
    ResponseBuffer resp = {NULL, 0};

    /*configured libcurl */
    CURL *curl = curl_easy_init();
    if (!curl) {
        free(req_json);
        return LLM_ERR_CURL;
    }

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Accept: application/json");

    curl_easy_setopt(curl, CURLOPT_URL,            g_url_ask);
    curl_easy_setopt(curl, CURLOPT_POST,            1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS,      req_json);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER,      headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,   write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,       &resp);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT,         (long)g_cfg.timeout_sec);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT,  5L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER,  0L);

    /* run the resquest */
    CURLcode res = curl_easy_perform(curl);

    /* result */
    LlmStatus status = LLM_OK;

    if (res != CURLE_OK) {
        fprintf(stderr,
            "[LLM] Erreur curl : %s\n"
            "  → llama-server est-il lancé ?\n"
            "  → Commande : ~/llama.cpp/build/bin/llama-server ...\n",
            curl_easy_strerror(res));
        status = LLM_ERR_CONNECT;

    } else {
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        if (http_code != 200) {
            fprintf(stderr, "[LLM] Serveur répond HTTP %ld\n", http_code);
            status = LLM_ERR_HTTP;
        } else {
            bool ok = extract_content(resp.data, out_buf, out_len);
            if (!ok) status = LLM_ERR_JSON;
        }
    }

    /* the cleanup (always, even if error) */
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    free(req_json);
    free(resp.data);

    return status;
}


typedef struct {
    char        *prompt;    
    LlmCallback  callback;
    void        *userdata;
} AsyncTask;


static gpointer thread_func(gpointer data) {
    AsyncTask *task = (AsyncTask *)data;

    char buf[4096] = {0};
    LlmStatus status = llm_client_ask(task->prompt, buf, sizeof(buf));

    if (task->callback) {
        task->callback(
            status,
            (status == LLM_OK) ? buf : NULL,
            task->userdata
        );
    }

   
    g_free(task->prompt);
    g_free(task);
    return NULL;
}

bool llm_client_ask_async(const char *prompt,
                           LlmCallback callback,
                           void       *userdata) {
    if (!g_initialized || !prompt || !prompt[0]) return false;


    AsyncTask *task = g_new(AsyncTask, 1);
    if (!task) return false;


    task->prompt   = g_strdup(prompt);
    task->callback = callback;
    task->userdata = userdata;

    GThread *t = g_thread_new("llm-worker", thread_func, task);
    if (!t) {
        g_free(task->prompt);
        g_free(task);
        return false;
    }


    g_thread_unref(t);
    return true;
}

bool llm_rephrase_async(const char *sentence,
                         LlmCallback callback,
                         void       *userdata) {
    if (!sentence) return false;

    
    int n = snprintf(NULL, 0, PROMPT_REPHRASE, sentence);
    if (n <= 0) return false;

    char *prompt = g_malloc(n + 1);
    if (!prompt) return false;

    snprintf(prompt, n + 1, PROMPT_REPHRASE, sentence);

    bool ok = llm_client_ask_async(prompt, callback, userdata);
    g_free(prompt);
    return ok;
}

bool llm_grammar_async(const char *text,
                        LlmCallback callback,
                        void       *userdata) {
    if (!text) return false;

    int n = snprintf(NULL, 0, PROMPT_GRAMMAR, text);
    if (n <= 0) return false;

    char *prompt = g_malloc(n + 1);
    if (!prompt) return false;

    snprintf(prompt, n + 1, PROMPT_GRAMMAR, text);

    bool ok = llm_client_ask_async(prompt, callback, userdata);
    g_free(prompt);
    return ok;
}

bool llm_semantic_async(const char *question,
                         const char *text,
                         LlmCallback callback,
                         void       *userdata) {
    if (!question || !text) return false;

    int n = snprintf(NULL, 0, PROMPT_SEMANTIC, question, text);
    if (n <= 0) return false;

    char *prompt = g_malloc(n + 1);
    if (!prompt) return false;

    snprintf(prompt, n + 1, PROMPT_SEMANTIC, question, text);

    bool ok = llm_client_ask_async(prompt, callback, userdata);
    g_free(prompt);
    return ok;
}

const char *llm_status_string(LlmStatus s) {
    switch (s) {
        case LLM_OK:           return "Succès";
        case LLM_ERR_NOT_INIT: return "init() non appelé";
        case LLM_ERR_CURL:     return "libcurl non disponible";
        case LLM_ERR_CONNECT:  return "serveur injoignable (lancé ?)";
        case LLM_ERR_HTTP:     return "réponse HTTP inattendue";
        case LLM_ERR_JSON:     return "parsing JSON échoué";
        case LLM_ERR_EMPTY:    return "prompt vide";
        case LLM_ERR_ALLOC:    return "erreur mémoire";
        default:               return "erreur inconnue";
    }
}