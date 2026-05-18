#include "config/config_parser.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *trim_inplace(char *s) {
    if (!s) {
        return s;
    }

    while (*s && isspace((unsigned char)*s)) {
        s++;
    }

    if (*s == '\0') {
        return s;
    }

    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }

    return s;
}

static void copy_string_field(char *dest, size_t dest_size, const char *value) {
    if (!dest || dest_size == 0 || !value) {
        return;
    }
    snprintf(dest, dest_size, "%s", value);
}

static void apply_key_value(AppConfig *cfg, const char *key, const char *value) {
    if (!cfg || !key || !value) {
        return;
    }

    if (strcmp(key, "theme") == 0 || strcmp(key, "Theme") == 0) {
        copy_string_field(cfg->theme, sizeof(cfg->theme), value);
    } else if (strcmp(key, "autosave_interval") == 0) {
        unsigned long parsed = strtoul(value, NULL, 10);
        cfg->autosave_interval_sec = (unsigned int)parsed;
    } else if (strcmp(key, "host") == 0) {
        copy_string_field(cfg->llm_host, sizeof(cfg->llm_host), value);
    } else if (strcmp(key, "port") == 0) {
        cfg->llm_port = (int)strtol(value, NULL, 10);
    }
}

void app_config_set_defaults(AppConfig *cfg) {
    if (!cfg) {
        return;
    }

    copy_string_field(cfg->theme, sizeof(cfg->theme), "light");
    cfg->autosave_interval_sec = 0;
    copy_string_field(cfg->llm_host, sizeof(cfg->llm_host), "127.0.0.1");
    cfg->llm_port = 8080;
}

bool app_config_load(const char *path, AppConfig *cfg) {
    if (!cfg) {
        return false;
    }

    app_config_set_defaults(cfg);

    if (!path) {
        return false;
    }

    FILE *fp = fopen(path, "r");
    if (!fp) {
        return false;
    }

    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        char *cursor = line;

        while (*cursor == ' ' || *cursor == '\t') {
            cursor++;
        }

        if (*cursor == '\0' || *cursor == '\n' || *cursor == '#' || *cursor == ';') {
            continue;
        }

        if (*cursor == '[') {
            continue;
        }

        char *eq = strchr(cursor, '=');
        if (!eq) {
            continue;
        }

        *eq = '\0';
        char *key = trim_inplace(cursor);
        char *value = trim_inplace(eq + 1);

        if (*key == '\0') {
            continue;
        }

        apply_key_value(cfg, key, value);
    }

    fclose(fp);

    if (cfg->llm_port <= 0 || cfg->llm_port > 65535) {
        cfg->llm_port = 8080;
    }

    return true;
}
