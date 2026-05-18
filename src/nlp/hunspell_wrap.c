/**
 * hunspell_wrap.c
 * Hunspell wrapper implementation — Fedora 43
 *
 * STRUCTURE:
 *   Section 1: Internal state
 *   Section 2: Dynamic SpellResult array
 *   Section 3: Init / Cleanup
 *   Section 4: Word checking
 *   Section 5: Suggestions
 *   Section 6: UTF-8 tokenizer
 *   Section 7: Full text checking
 */

#define _GNU_SOURCE

#include "nlp/hunspell_wrap.h"
#include <hunspell/hunspell.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


static Hunhandle *g_hunspell = NULL;
static bool       g_ready    = false;


static bool spell_result_append(SpellResult *r, const SpellError *e) {
    if (r->capacity == 0) {
        r->errors = malloc(8 * sizeof(SpellError));
        if (!r->errors) return false;
        r->capacity = 8;
        r->count    = 0;
    } else if (r->count >= r->capacity) {
        int new_cap = r->capacity * 2;
        SpellError *tmp = realloc(r->errors, new_cap * sizeof(SpellError));
        if (!tmp) return false;
        r->errors   = tmp;
        r->capacity = new_cap;
    }
    r->errors[r->count++] = *e;
    return true;
}

void spell_result_free(SpellResult *r) {
    if (!r) return;
    free(r->errors);
    r->errors   = NULL;
    r->count    = 0;
    r->capacity = 0;
}


bool hunspell_wrap_init(const char *aff_path, const char *dic_path) {
    if (!aff_path || !dic_path) return false;

    /*
     * REQUIRED ORDER: AFF first, DIC second.
     * Reversing them makes Hunspell load without error but
     * it does not recognize any word.
     */
    g_hunspell = Hunspell_create(aff_path, dic_path);
    if (!g_hunspell) {
        fprintf(stderr,
            "[Hunspell] Échec chargement.\n"
            "  .aff : %s\n"
            "  .dic : %s\n"
            "  → sudo dnf install hunspell-fr\n",
            aff_path, dic_path);
        return false;
    }

    g_ready = true;
    fprintf(stdout, "[Hunspell] Dictionnaire fr_FR chargé.\n");
    return true;
}

void hunspell_wrap_cleanup(void) {
    if (g_hunspell) {
        Hunspell_destroy(g_hunspell);
        g_hunspell = NULL;
    }
    g_ready = false;
}

bool hunspell_wrap_is_ready(void) {
    return g_ready;
}


bool hunspell_wrap_check(const char *word) {
    if (!g_ready || !word || !word[0]) return true;
    /*
     * Hunspell_spell() returns:
     *   0 → INCORRECT WORD
     *   1 → correct word
     */
    return Hunspell_spell(g_hunspell, word) != 0;
}


int hunspell_wrap_suggest(const char *word, char **out, int max_sugg) {
    if (!g_ready || !word || !out || max_sugg <= 0) return 0;

    char **h_list = NULL;
    int    n      = Hunspell_suggest(g_hunspell, &h_list, word);
    int    count  = (n < max_sugg) ? n : max_sugg;

    for (int i = 0; i < count; i++) {
        out[i] = strdup(h_list[i]);
        if (!out[i]) {
            for (int j = 0; j < i; j++) free(out[j]);
            Hunspell_free_list(g_hunspell, &h_list, n);
            return 0;
        }
    }

    Hunspell_free_list(g_hunspell, &h_list, n);
    return count;
}

void hunspell_wrap_free_suggestions(char **sugg, int count) {
    if (!sugg) return;
    for (int i = 0; i < count; i++) {
        free(sugg[i]);
        sugg[i] = NULL;
    }
}

void hunspell_wrap_add_word(const char *word) {
    if (!g_ready || !word) return;
    Hunspell_add(g_hunspell, word);
}



/*
 * WHY unsigned char?
 * In C, char is signed on Linux. UTF-8 bytes for accented letters
 * (é=0xC3, à=0xC3...) have values > 127.
 * With signed char, 0xC3 = -61. The comparison c >= 0x80 would be false.
 * With unsigned char, 0xC3 = 195. The comparison 195 >= 128 is correct.
 */
static bool is_word_byte(unsigned char c) {
    return isalpha(c) || c >= 0x80;
}


int hunspell_wrap_check_text(const char *text, SpellResult *result) {
    if (!g_ready || !text || !result) return -1;

    result->errors   = NULL;
    result->count    = 0;
    result->capacity = 0;

    int len = (int)strlen(text);
    int i   = 0;

    while (i < len) {
        unsigned char c = (unsigned char)text[i];

        /* ── Start of a word */
        if (is_word_byte(c)) {
            int word_start = i;

            while (i < len) {
                unsigned char cc = (unsigned char)text[i];
                if (is_word_byte(cc)) {
                    i++;
                } else if (cc == '\'' && i + 1 < len
                           && is_word_byte((unsigned char)text[i + 1])) {
                    /* Apostrophe in "j'ai" → stop before it */
                    break;
                } else if (cc == '-' && i + 1 < len
                           && is_word_byte((unsigned char)text[i + 1])) {
                    /* Hyphen in "arc-en-ciel" → continue */
                    i++;
                } else {
                    break;
                }
            }

            int word_len = i - word_start;
            if (word_len <= 1 || word_len >= SPELL_MAX_WORD_LEN)
                continue;

            char word_buf[SPELL_MAX_WORD_LEN];
            strncpy(word_buf, text + word_start, word_len);
            word_buf[word_len] = '\0';

            if (!hunspell_wrap_check(word_buf)) {
                SpellError err;
                memset(&err, 0, sizeof(err));
                err.start  = word_start;
                err.length = word_len;
                strncpy(err.word, word_buf, SPELL_MAX_WORD_LEN - 1);

                char *sugg[SPELL_MAX_SUGGESTIONS] = {NULL};
                int ns = hunspell_wrap_suggest(word_buf, sugg,
                                               SPELL_MAX_SUGGESTIONS);
                err.n_suggestions = ns;
                for (int s = 0; s < ns; s++) {
                    if (sugg[s])
                        strncpy(err.suggestions[s], sugg[s],
                                SPELL_MAX_WORD_LEN - 1);
                }
                hunspell_wrap_free_suggestions(sugg, ns);
                spell_result_append(result, &err);
            }
        }
        /* UTF-8 typographic apostrophe U+2019 = 0xE2 0x80 0x99 */
        else if (c == 0xE2 && i + 2 < len
                 && (unsigned char)text[i+1] == 0x80
                 && (unsigned char)text[i+2] == 0x99) {
            i += 3;
        } else {
            i++;
        }
    }

    return result->count;
}