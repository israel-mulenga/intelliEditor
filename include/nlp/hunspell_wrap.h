

#ifndef HUNSPELL_WRAP_H
#define HUNSPELL_WRAP_H

#include <stdbool.h>
#include <stddef.h>


#define SPELL_MAX_SUGGESTIONS  8
#define SPELL_MAX_WORD_LEN     256


typedef struct {
    int  start;  
    int  length;  
    char word[SPELL_MAX_WORD_LEN];
    char suggestions[SPELL_MAX_SUGGESTIONS][SPELL_MAX_WORD_LEN];
    int  n_suggestions;
} SpellError;


typedef struct {
    SpellError *errors;
    int         count;
    int         capacity;
} SpellResult;

/* FUNCTIONS                                                         */

/**
 * hunspell_wrap_init() — load the French dictionary.
 *
 * @param aff_path  path to the .aff file
 * @param dic_path  path to the .dic file
 * @return          true on success
 */
bool hunspell_wrap_init(const char *aff_path, const char *dic_path);

/** hunspell_wrap_cleanup() — release Hunspell resources. */
void hunspell_wrap_cleanup(void);

/** hunspell_wrap_is_ready() — return true if Hunspell is initialized. */
bool hunspell_wrap_is_ready(void);

/**
 * hunspell_wrap_check() — check a single word's spelling.
 * @param word    word to check (UTF-8)
 * @return        true if the word is spelled correctly, false otherwise
 */
bool hunspell_wrap_check(const char *word);


int  hunspell_wrap_suggest(const char *word, char **out, int max_sugg);
void hunspell_wrap_free_suggestions(char **sugg, int count);

void hunspell_wrap_add_word(const char *word);

/**
 * hunspell_wrap_check_text() — check the spelling of a full text.
 *
 * Tokenizes the text, checks each word, and fills the result list.
 *
 * @param text    complete UTF-8 text to check
 * @param result  structure to fill (initialized by this function)
 * @return        number of errors found, -1 if Hunspell not ready
 */
int  hunspell_wrap_check_text(const char *text, SpellResult *result);

/** spell_result_free() — free the memory of a SpellResult. */
void spell_result_free(SpellResult *result);

#endif /* HUNSPELL_WRAP_H */