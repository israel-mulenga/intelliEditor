#include "rules/rules.h"
#include "cJSON.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int parse_min_words_from_parameter(const char *parameter, int *min_words) {
    if (!parameter || !min_words) {
        return 0;
    }

    cJSON *json = cJSON_Parse(parameter);
    if (json) {
        const cJSON *min_item = cJSON_GetObjectItemCaseSensitive(json, "min_words");
        if (cJSON_IsNumber(min_item)) {
            *min_words = min_item->valueint;
            cJSON_Delete(json);
            return 1;
        }
        cJSON_Delete(json);
    }

    char *end = NULL;
    long value = strtol(parameter, &end, 10);
    if (end != parameter && value >= 0) {
        *min_words = (int)value;
        return 1;
    }

    return 0;
}

static int parse_max_words_from_parameter(const char *parameter, int *max_words) {
    if (!parameter || !max_words) {
        return 0;
    }

    cJSON *json = cJSON_Parse(parameter);
    if (!json) {
        return 0;
    }

    const cJSON *max_item = cJSON_GetObjectItemCaseSensitive(json, "max_words");
    if (cJSON_IsNumber(max_item)) {
        *max_words = max_item->valueint;
        cJSON_Delete(json);
        return 1;
    }

    cJSON_Delete(json);
    return 0;
}

static int text_contains_section(const char *text, const char *section_name) {
    if (!text || !section_name || !*section_name) {
        return 0;
    }

    return strstr(text, section_name) != NULL;
}

static RuleStatus evaluate_rule(Rule *rule, const char *text) {
    if (!rule || !text) {
        return STATUS_PENDING;
    }

    if (strcmp(rule->check_type, "word_count_min") == 0) {
        int min_words = 0;
        if (!parse_min_words_from_parameter(rule->parameter, &min_words)) {
            return STATUS_PENDING;
        }
        return check_word_count_min(text, min_words) ? STATUS_OK : STATUS_ERROR;
    }

    if (strcmp(rule->check_type, "word_count_max") == 0) {
        int max_words = 0;
        if (!parse_max_words_from_parameter(rule->parameter, &max_words)) {
            return STATUS_PENDING;
        }
        return count_words(text) <= max_words ? STATUS_OK : STATUS_WARNING;
    }

    if (strcmp(rule->check_type, "regex_forbidden") == 0) {
        if (!rule->parameter || !*rule->parameter) {
            return STATUS_PENDING;
        }
        return check_forbidden_regex(text, rule->parameter) ? STATUS_ERROR : STATUS_OK;
    }

    if (strcmp(rule->check_type, "section_exists") == 0) {
        return text_contains_section(text, rule->parameter) ? STATUS_OK : STATUS_ERROR;
    }

    if (strcmp(rule->check_type, "section_order") == 0 ||
        strcmp(rule->check_type, "heading_format") == 0 ||
        strcmp(rule->check_type, "regex_required") == 0 ||
        strcmp(rule->check_type, "citation_present") == 0 ||
        strcmp(rule->check_type, "llm_semantic") == 0) {
        return STATUS_PENDING;
    }

    return STATUS_PENDING;
}

int ruleset_evaluate(RuleSet *ruleset, const char *text) {
    if (!ruleset || !text) {
        return 0;
    }

    for (int i = 0; i < ruleset->rule_count; i++) {
        ruleset->rules[i].status = evaluate_rule(&ruleset->rules[i], text);
    }

    return ruleset->rule_count;
}

int ruleset_count_by_status(const RuleSet *ruleset, RuleStatus status) {
    if (!ruleset) {
        return 0;
    }

    int count = 0;
    for (int i = 0; i < ruleset->rule_count; i++) {
        if (ruleset->rules[i].status == status) {
            count++;
        }
    }
    return count;
}
