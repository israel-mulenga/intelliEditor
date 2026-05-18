/**
 * nlp_engine.c
 * 
 * Orchestration of Hunspell + quick rules.
 */

#define _GNU_SOURCE

#include "nlp/nlp_engine.h"
#include "nlp/hunspell_wrap.h"
#include "llm/llm_client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ERROR LIST MANAGEMENT */


void nlp_error_list_init(NlpErrorList *list) {
    if (!list) return;
    list->items    = NULL;
    list->count    = 0;
    list->capacity = 0;
}

void nlp_error_list_free(NlpErrorList *list) {
    if (!list) return;
    free(list->items);
    list->items    = NULL;
    list->count    = 0;
    list->capacity = 0;
}

void nlp_error_list_merge(NlpErrorList *dst, const NlpErrorList *src) {
    if (!dst || !src) return;
    for (int i = 0; i < src->count; i++) {
        if (dst->count >= dst->capacity) {
            int nc = (dst->capacity == 0) ? 8 : dst->capacity * 2;
            NlpError *nb = realloc(dst->items, nc * sizeof(NlpError));
            if (!nb) return;
            dst->items    = nb;
            dst->capacity = nc;
        }
        dst->items[dst->count++] = src->items[i];
    }
}

static bool list_push(NlpErrorList *list, const NlpError *err) {
    if (list->count >= list->capacity) {
        int nc = (list->capacity == 0) ? 8 : list->capacity * 2;
        NlpError *nb = realloc(list->items, nc * sizeof(NlpError));
        if (!nb) return false;
        list->items    = nb;
        list->capacity = nc;
    }
    list->items[list->count++] = *err;
    return true;
}


/* INIT / CLEANUP */


bool nlp_engine_init(const char *aff_path, const char *dic_path,
                     const char *llm_host, int llm_port) {

    bool hunspell_ok = hunspell_wrap_init(aff_path, dic_path);
    if (!hunspell_ok) {
        fprintf(stderr,
            "[NLP] Hunspell non disponible.\n"
            "  → sudo dnf install hunspell-fr\n"
            "  → Dictionnaires : /usr/share/myspell/fr_FR.*\n");
    }

    LlmConfig cfg;
    llm_config_default(&cfg);
    if (llm_host) strncpy(cfg.host, llm_host, sizeof(cfg.host) - 1);
    if (llm_port > 0) cfg.port = llm_port;

    bool llm_ok = llm_client_init(&cfg);
    if (llm_ok && llm_client_server_up()) {
        fprintf(stdout, "[NLP] llama-server opérationnel.\n");
    } else {
        fprintf(stdout,
            "[NLP] llama-server non détecté.\n"
            "  → L'orthographe fonctionne sans le LLM.\n");
    }

    return hunspell_ok;
}

void nlp_engine_cleanup(void) {
    hunspell_wrap_cleanup();
    llm_client_cleanup();
}


/* SPELLING CORRECTION */


int nlp_check_spelling(const char *text, NlpErrorList *out) {
    if (!text || !out) return -1;
    nlp_error_list_init(out);
    if (!hunspell_wrap_is_ready()) return -1;

    SpellResult spell;
    int n = hunspell_wrap_check_text(text, &spell);
    if (n < 0) return -1;

    for (int i = 0; i < spell.count; i++) {
        SpellError *se = &spell.errors[i];
        NlpError ne;
        memset(&ne, 0, sizeof(ne));
        ne.start  = se->start;
        ne.length = se->length;
        ne.type   = NLP_ERR_SPELLING;
        strncpy(ne.original, se->word, sizeof(ne.original) - 1);
        if (se->n_suggestions > 0)
            strncpy(ne.suggestion, se->suggestions[0],
                    sizeof(ne.suggestion) - 1);
        strncpy(ne.explanation,
                "Mot non reconnu dans le dictionnaire français",
                sizeof(ne.explanation) - 1);
        list_push(out, &ne);
    }

    spell_result_free(&spell);
    return out->count;
}

/* FAST RULES IN C */


typedef struct { const char *wrong; const char *correct; } WordPair;

static const WordPair ANGLICISMS[] = {
    { "mail",         "courriel"         },
    { "email",        "courriel"         },
    { "e-mail",       "courriel"         },
    { "meeting",      "réunion"          },
    { "deadline",     "date limite"      },
    { "feedback",     "retour"           },
    { "workshop",     "atelier"          },
    { "manager",      "responsable"      },
    { "online",       "en ligne"         },
    { "offline",      "hors ligne"       },
    { "update",       "mise à jour"      },
    { "bug",          "bogue"            },
    { "newsletter",   "lettre d'info"    },
    { "brainstorming","remue-méninges"   },
    { "checklist",    "liste de contrôle"},
    { NULL, NULL }
};

static int check_anglicisms(const char *text, NlpErrorList *list) {
    int found = 0;
    int tlen  = (int)strlen(text);

    for (int a = 0; ANGLICISMS[a].wrong != NULL; a++) {
        const char *wrong = ANGLICISMS[a].wrong;
        int         wlen  = (int)strlen(wrong);

        for (int pos = 0; pos <= tlen - wlen; pos++) {
            bool match = true;
            for (int k = 0; k < wlen && match; k++) {
                if (tolower((unsigned char)text[pos + k]) !=
                    tolower((unsigned char)wrong[k]))
                    match = false;
            }
            if (!match) continue;

            bool left_ok  = (pos == 0 ||
                             !isalpha((unsigned char)text[pos - 1]));
            bool right_ok = (pos + wlen >= tlen ||
                             !isalpha((unsigned char)text[pos + wlen]));
            if (!left_ok || !right_ok) continue;

            NlpError e;
            memset(&e, 0, sizeof(e));
            e.start  = pos;
            e.length = wlen;
            e.type   = NLP_ERR_ANGLICISM;
            strncpy(e.original,    wrong,                 sizeof(e.original)   - 1);
            strncpy(e.suggestion,  ANGLICISMS[a].correct, sizeof(e.suggestion) - 1);
            strncpy(e.explanation, "Anglicisme — préférer le terme français",
                    sizeof(e.explanation) - 1);
            list_push(list, &e);
            found++;
        }
    }
    return found;
}

static int check_punctuation_fr(const char *text, NlpErrorList *list) {
    int found = 0;
    int len   = (int)strlen(text);
    for (int i = 1; i < len; i++) {
        unsigned char c = (unsigned char)text[i];
        if ((c == ':' || c == '?' || c == '!' || c == ';')
            && text[i - 1] != ' ') {
            NlpError e;
            memset(&e, 0, sizeof(e));
            e.start       = i;
            e.length      = 1;
            e.type        = NLP_ERR_STYLE;
            e.original[0] = (char)c;
            snprintf(e.suggestion,  sizeof(e.suggestion),  " %c", c);
            snprintf(e.explanation, sizeof(e.explanation),
                     "Espace obligatoire avant '%c' en français", c);
            list_push(list, &e);
            found++;
        }
    }
    return found;
}

static int check_repetitions(const char *text, NlpErrorList *list) {
    int found = 0;
    int len   = (int)strlen(text);

#define WIN 4
    typedef struct { int start; int len; } Wrd;
    Wrd window[WIN] = {{0,0}};
    int widx = 0, wcnt = 0, i = 0;

    while (i < len) {
        unsigned char c = (unsigned char)text[i];
        if (!isalpha(c) && c < 0x80) { i++; continue; }

        int ws = i;
        while (i < len && (isalpha((unsigned char)text[i])
                           || (unsigned char)text[i] >= 0x80)) i++;
        int wl = i - ws;
        if (wl < 2) continue;

        for (int k = 0; k < wcnt; k++) {
            int idx = (widx - 1 - k + WIN) % WIN;
            if (window[idx].len == wl
                && strncasecmp(text + ws, text + window[idx].start, wl) == 0) {
                NlpError e;
                memset(&e, 0, sizeof(e));
                e.start  = ws;
                e.length = wl;
                e.type   = NLP_ERR_REPEAT;
                int cp = wl < (int)sizeof(e.original) - 1 ? wl
                                                           : (int)sizeof(e.original) - 1;
                strncpy(e.original, text + ws, cp);
                strncpy(e.suggestion,  "Utiliser un synonyme",
                        sizeof(e.suggestion)  - 1);
                snprintf(e.explanation, sizeof(e.explanation),
                         "Mot répété à moins de %d mots d'écart", WIN);
                list_push(list, &e);
                found++;
                break;
            }
        }

        window[widx] = (Wrd){ws, wl};
        widx  = (widx + 1) % WIN;
        wcnt  = (wcnt < WIN) ? wcnt + 1 : WIN;
    }
#undef WIN
    return found;
}

int nlp_check_fast_rules(const char *text, NlpErrorList *out) {
    if (!text || !out) return -1;
    nlp_error_list_init(out);
    int total = 0;
    total += check_anglicisms    (text, out);
    total += check_punctuation_fr(text, out);
    total += check_repetitions   (text, out);
    return total;
}

int nlp_check_all_sync(const char *text, NlpErrorList *out) {
    if (!text || !out) return -1;
    nlp_error_list_init(out);

    NlpErrorList spell, rules;
    nlp_check_spelling   (text, &spell);
    nlp_check_fast_rules (text, &rules);
    nlp_error_list_merge (out,  &spell);
    nlp_error_list_merge (out,  &rules);
    nlp_error_list_free  (&spell);
    nlp_error_list_free  (&rules);

    return out->count;
}