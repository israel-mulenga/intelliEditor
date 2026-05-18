#include "rules/rules.h"
#include "cJSON.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RULES_FILE_MAX_BYTES (2U * 1024U * 1024U)

static char *read_file_to_string(const char *filename, size_t *out_len) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return NULL;
    }

    long length = ftell(file);
    if (length < 0 || (unsigned long)length > RULES_FILE_MAX_BYTES) {
        fclose(file);
        return NULL;
    }

    if (fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return NULL;
    }

    char *data = malloc((size_t)length + 1);
    if (!data) {
        fclose(file);
        return NULL;
    }

    size_t read_bytes = fread(data, 1, (size_t)length, file);
    fclose(file);

    if (read_bytes != (size_t)length) {
        free(data);
        return NULL;
    }

    data[length] = '\0';
    if (out_len) {
        *out_len = (size_t)length;
    }
    return data;
}

static void copy_string_field(char *dest, size_t dest_size, const char *src) {
    if (!dest || dest_size == 0) {
        return;
    }
    if (!src) {
        dest[0] = '\0';
        return;
    }
    snprintf(dest, dest_size, "%s", src);
}

static char *parameter_from_json(const cJSON *value) {
    if (!value) {
        return strdup("");
    }
    if (cJSON_IsString(value)) {
        return strdup(value->valuestring);
    }
    char *printed = cJSON_PrintUnformatted(value);
    return printed ? printed : strdup("");
}

static int parse_meta(const cJSON *meta, RuleMeta *out) {
    if (!out) {
        return 0;
    }

    memset(out, 0, sizeof(*out));

    if (!cJSON_IsObject(meta)) {
        return 1;
    }

    const cJSON *document_type = cJSON_GetObjectItemCaseSensitive(meta, "document_type");
    const cJSON *version = cJSON_GetObjectItemCaseSensitive(meta, "version");
    const cJSON *author = cJSON_GetObjectItemCaseSensitive(meta, "author");
    const cJSON *description = cJSON_GetObjectItemCaseSensitive(meta, "description");

    if (cJSON_IsString(document_type)) {
        copy_string_field(out->document_type, sizeof(out->document_type), document_type->valuestring);
    }
    if (cJSON_IsString(version)) {
        copy_string_field(out->version, sizeof(out->version), version->valuestring);
    }
    if (cJSON_IsString(author)) {
        copy_string_field(out->author, sizeof(out->author), author->valuestring);
    }
    if (cJSON_IsString(description)) {
        copy_string_field(out->description, sizeof(out->description), description->valuestring);
    }

    return 1;
}

static int parse_rule_object(const cJSON *item, Rule *out) {
    if (!cJSON_IsObject(item) || !out) {
        return 0;
    }

    memset(out, 0, sizeof(*out));
    out->status = STATUS_PENDING;

    const cJSON *id = cJSON_GetObjectItemCaseSensitive(item, "id");
    const cJSON *category = cJSON_GetObjectItemCaseSensitive(item, "category");
    const cJSON *severity = cJSON_GetObjectItemCaseSensitive(item, "severity");
    const cJSON *description = cJSON_GetObjectItemCaseSensitive(item, "description");
    const cJSON *check_type = cJSON_GetObjectItemCaseSensitive(item, "check_type");
    const cJSON *parameter = cJSON_GetObjectItemCaseSensitive(item, "parameter");
    const cJSON *flags = cJSON_GetObjectItemCaseSensitive(item, "flags");
    const cJSON *target_section = cJSON_GetObjectItemCaseSensitive(item, "target_section");

    if (!cJSON_IsString(id) || !cJSON_IsString(check_type)) {
        return 0;
    }

    copy_string_field(out->id, sizeof(out->id), id->valuestring);
    copy_string_field(out->check_type, sizeof(out->check_type), check_type->valuestring);

    if (cJSON_IsString(category)) {
        copy_string_field(out->category, sizeof(out->category), category->valuestring);
    }
    if (cJSON_IsString(severity)) {
        copy_string_field(out->severity, sizeof(out->severity), severity->valuestring);
    }
    if (cJSON_IsString(description)) {
        copy_string_field(out->description, sizeof(out->description), description->valuestring);
    }
    if (cJSON_IsString(flags)) {
        copy_string_field(out->flags, sizeof(out->flags), flags->valuestring);
    }
    if (cJSON_IsString(target_section)) {
        copy_string_field(out->target_section, sizeof(out->target_section), target_section->valuestring);
    }

    out->parameter = parameter_from_json(parameter);
    if (!out->parameter) {
        return 0;
    }

    return 1;
}

static void free_rule(Rule *rule) {
    if (!rule) {
        return;
    }
    free(rule->parameter);
    rule->parameter = NULL;
}

const char *rules_default_file_path(void) {
    static const char *candidates[] = {
        "data/memoire_licence.json",
        "memoire_licence.json",
        NULL
    };

    for (int i = 0; candidates[i]; i++) {
        FILE *fp = fopen(candidates[i], "r");
        if (fp) {
            fclose(fp);
            return candidates[i];
        }
    }

    return NULL;
}

RuleSet *ruleset_load(const char *filename) {
    if (!filename) {
        return NULL;
    }

    char *data = read_file_to_string(filename, NULL);
    if (!data) {
        fprintf(stderr, "ruleset: cannot read '%s'\n", filename);
        return NULL;
    }

    cJSON *root = cJSON_Parse(data);
    free(data);
    if (!root) {
        fprintf(stderr, "ruleset: invalid JSON in '%s'\n", filename);
        return NULL;
    }

    const cJSON *rules_array = cJSON_GetObjectItemCaseSensitive(root, "rules");
    if (!cJSON_IsArray(rules_array)) {
        fprintf(stderr, "ruleset: missing 'rules' array in '%s'\n", filename);
        cJSON_Delete(root);
        return NULL;
    }

    int count = cJSON_GetArraySize(rules_array);
    RuleSet *ruleset = calloc(1, sizeof(RuleSet));
    if (!ruleset) {
        cJSON_Delete(root);
        return NULL;
    }

    const cJSON *meta = cJSON_GetObjectItemCaseSensitive(root, "meta");
    parse_meta(meta, &ruleset->meta);

    if (count > 0) {
        ruleset->rules = calloc((size_t)count, sizeof(Rule));
        if (!ruleset->rules) {
            ruleset_free(ruleset);
            cJSON_Delete(root);
            return NULL;
        }
    }

    int written = 0;
    for (int i = 0; i < count; i++) {
        const cJSON *item = cJSON_GetArrayItem(rules_array, i);
        if (parse_rule_object(item, &ruleset->rules[written])) {
            written++;
        } else {
            free_rule(&ruleset->rules[written]);
        }
    }

    ruleset->rule_count = written;
    cJSON_Delete(root);
    return ruleset;
}

void ruleset_free(RuleSet *ruleset) {
    if (!ruleset) {
        return;
    }

    for (int i = 0; i < ruleset->rule_count; i++) {
        free_rule(&ruleset->rules[i]);
    }
    free(ruleset->rules);
    free(ruleset);
}

const char *rule_status_icon(RuleStatus status) {
    switch (status) {
    case STATUS_OK: return "OK";
    case STATUS_WARNING: return "WARN";
    case STATUS_ERROR: return "ERR";
    case STATUS_PENDING: return "PENDING";
    default: return "?";
    }
}

const char *rule_status_label(RuleStatus status) {
    switch (status) {
    case STATUS_OK: return "Conforme";
    case STATUS_WARNING: return "Avertissement";
    case STATUS_ERROR: return "Non conforme";
    case STATUS_PENDING: return "En cours";
    default: return "Inconnu";
    }
}

ConfigRules *parse_config(const char *filename) {
    char *data = read_file_to_string(filename, NULL);
    if (!data) {
        return NULL;
    }

    cJSON *json = cJSON_Parse(data);
    free(data);
    if (!json) {
        return NULL;
    }

    if (cJSON_GetObjectItemCaseSensitive(json, "rules")) {
        cJSON_Delete(json);
        return NULL;
    }

    ConfigRules *config = calloc(1, sizeof(ConfigRules));
    if (!config) {
        cJSON_Delete(json);
        return NULL;
    }

    const cJSON *min_words = cJSON_GetObjectItemCaseSensitive(json, "min_words");
    if (cJSON_IsNumber(min_words)) {
        config->min_words = min_words->valueint;
    }

    const cJSON *patterns = cJSON_GetObjectItemCaseSensitive(json, "forbidden_patterns");
    if (cJSON_IsArray(patterns)) {
        config->forbidden_count = cJSON_GetArraySize(patterns);
        if (config->forbidden_count > 0) {
            config->forbidden_patterns = calloc((size_t)config->forbidden_count, sizeof(char *));
            for (int i = 0; i < config->forbidden_count; i++) {
                const cJSON *item = cJSON_GetArrayItem(patterns, i);
                if (cJSON_IsString(item)) {
                    config->forbidden_patterns[i] = strdup(item->valuestring);
                }
            }
        }
    }

    cJSON_Delete(json);
    return config;
}

void free_config(ConfigRules *config) {
    if (!config) {
        return;
    }

    if (config->forbidden_patterns) {
        for (int i = 0; i < config->forbidden_count; i++) {
            free(config->forbidden_patterns[i]);
        }
        free(config->forbidden_patterns);
    }
    free(config);
}
