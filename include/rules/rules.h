#ifndef RULES_H
#define RULES_H

#include <stddef.h>

typedef enum {
    STATUS_OK,
    STATUS_WARNING,
    STATUS_ERROR,
    STATUS_PENDING
} RuleStatus;

typedef struct {
    char document_type[100];
    char version[32];
    char author[128];
    char description[256];
} RuleMeta;

typedef struct {
    char id[16];
    char category[32];
    char severity[16];
    char description[256];
    char check_type[64];
    char *parameter;
    char flags[32];
    char target_section[128];
    RuleStatus status;
} Rule;

typedef struct {
    RuleMeta meta;
    Rule *rules;
    int rule_count;
} RuleSet;

typedef struct {
    int min_words;
    char **forbidden_patterns;
    int forbidden_count;
} ConfigRules;

const char *rule_status_icon(RuleStatus status);
const char *rule_status_label(RuleStatus status);

const char *rules_default_file_path(void);

RuleSet *ruleset_load(const char *filename);
void ruleset_free(RuleSet *ruleset);

int ruleset_evaluate(RuleSet *ruleset, const char *text);
int ruleset_count_by_status(const RuleSet *ruleset, RuleStatus status);

ConfigRules *parse_config(const char *filename);
void free_config(ConfigRules *config);

int check_forbidden_regex(const char *text, const char *pattern);
int count_words(const char *text);
int check_word_count_min(const char *text, int min_words);

#endif
