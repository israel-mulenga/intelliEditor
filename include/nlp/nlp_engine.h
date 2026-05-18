
#ifndef NLP_ENGINE_H
#define NLP_ENGINE_H

#include <stdbool.h>
#include <stddef.h>


/* ERROR TYPES                                                       */


/**
 * NlpErrorType — catégorie de l'erreur.
 * Détermine la couleur du soulignement dans GtkSourceView.
 */
typedef enum {
    NLP_ERR_SPELLING  = 0,  /* red     — spelling     */
    NLP_ERR_GRAMMAR   = 1,  /* orange  — grammar      */
    NLP_ERR_STYLE     = 2,  /* blue    — punctuation  */
    NLP_ERR_ANGLICISM = 3,  /* yellow  — anglicism    */
    NLP_ERR_REPEAT    = 4   /* purple  — repetition   */
} NlpErrorType;


/* NLP ERROR STRUCTURE                                               */


/**
 * NlpError — an error detected in the text.
 *
 * start/length are in bytes (UTF-8).
 * start == -1 means "unknown position" (LLM-generated errors).
 */
typedef struct {
    int          start;
    int          length;
    NlpErrorType type;
    char         original[256];
    char         suggestion[512];
    char         explanation[256];
} NlpError;

/* ERROR LIST (dynamic array)                                        */


typedef struct {
    NlpError *items;
    int       count;
    int       capacity;
} NlpErrorList;

/* INIT / CLEANUP                                                     */


/**
 * nlp_engine_init() — initialize Hunspell + LLM client.
 *
 * Fedora paths:
 *   /usr/share/myspell/fr_FR.aff
 *   /usr/share/myspell/fr_FR.dic
 *
 * @return true if Hunspell loaded successfully (LLM optional)
 */
bool nlp_engine_init(const char *aff_path,
                     const char *dic_path,
                     const char *llm_host,
                     int         llm_port);

void nlp_engine_cleanup(void);


/* SYNCHRONOUS CHECKS                                                 */


/**
 * nlp_check_spelling() — spelling check via Hunspell.
 * @return number of errors, -1 if Hunspell not ready
 */
int nlp_check_spelling(const char *text, NlpErrorList *out);

/**
 * nlp_check_fast_rules() — anglicisms, punctuation, repetitions.
 * No external dependencies — always available.
 * @return number of errors
 */
int nlp_check_fast_rules(const char *text, NlpErrorList *out);

/**
 * nlp_check_all_sync() — spelling + fast rules together.
 * This is the main function to call on each text change.
 * @return total number of errors
 */
int nlp_check_all_sync(const char *text, NlpErrorList *out);

/* ERROR LIST MANAGEMENT                                              */


void nlp_error_list_init (NlpErrorList *list);
void nlp_error_list_free (NlpErrorList *list);
void nlp_error_list_merge(NlpErrorList *dst, const NlpErrorList *src);

#endif /* NLP_ENGINE_H */