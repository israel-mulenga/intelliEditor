#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include <stdio.h>
#include <string.h>
#include "rules/rules.h"

// ============================================================================
// 1. VÉRIFICATION DES EXPRESSIONS RÉGULIÈRES (PCRE2)
// ============================================================================
int check_forbidden_regex(const char *text, const char *pattern) {
    if (!text || !pattern) return 0;

    pcre2_code *re;
    int errornumber;
    PCRE2_SIZE erroroffset;
    
    re = pcre2_compile(
        (PCRE2_SPTR)pattern,
        PCRE2_ZERO_TERMINATED,
        PCRE2_CASELESS,
        &errornumber,
        &erroroffset,
        NULL
    );
    
    if (re == NULL) {
        PCRE2_UCHAR buffer[256];
        pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
        printf("Erreur Regex pour '%s': %s\n", pattern, buffer);
        return 0;
    }

    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, NULL);
    
    int rc = pcre2_match(
        re,
        (PCRE2_SPTR)text,
        strlen(text),
        0,
        0,
        match_data,
        NULL
    );

    pcre2_match_data_free(match_data);
    pcre2_code_free(re);

    return (rc >= 0);
}

// ============================================================================
// 2. COMPTAGE ET VÉRIFICATION DES MOTS
// ============================================================================
int count_words(const char *text) {
    if (!text) return 0;

    int count = 0;
    int in_word = 0;

    while (*text) {
        // Détection des espaces, tabulations et retours à la ligne
        if (*text == ' ' || *text == '\t' || *text == '\n' || *text == '\r') {
            in_word = 0;
        } 
        // Début d'un nouveau mot détecté
        else if (!in_word) {
            in_word = 1;
            count++;
        }
        text++;
    }
    return count;
}

int check_word_count_min(const char *text, int min_words) {
    int current_count = count_words(text);
    // Renvoie 1 si le nombre de mots est suffisant, 0 sinon
    return (current_count >= min_words);
}