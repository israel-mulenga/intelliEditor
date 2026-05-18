#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <stdbool.h>
#include <stddef.h>

#define APP_CONFIG_THEME_MAX 32
#define APP_CONFIG_HOST_MAX 128

typedef struct {
    char theme[APP_CONFIG_THEME_MAX];
    unsigned int autosave_interval_sec;
    char llm_host[APP_CONFIG_HOST_MAX];
    int llm_port;
} AppConfig;

void app_config_set_defaults(AppConfig *cfg);

bool app_config_load(const char *path, AppConfig *cfg);

#endif
