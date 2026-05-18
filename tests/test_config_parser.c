#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "config/config_parser.h"

static void write_temp_ini(const char *path, const char *body) {
    FILE *fp = fopen(path, "w");
    assert(fp != NULL);
    fputs(body, fp);
    fclose(fp);
}

int main(void) {
    AppConfig cfg;
    const char *path = "/tmp/intellieditor_config_test.ini";

    app_config_set_defaults(&cfg);
    assert(strcmp(cfg.theme, "light") == 0);
    assert(cfg.autosave_interval_sec == 0);
    assert(strcmp(cfg.llm_host, "127.0.0.1") == 0);
    assert(cfg.llm_port == 8080);

    write_temp_ini(path,
        "[General]\n"
        "Theme=dark\n"
        "autosave_interval=45\n"
        "[LLM]\n"
        "host=10.0.0.5\n"
        "port=9000\n");

    assert(app_config_load(path, &cfg));
    assert(strcmp(cfg.theme, "dark") == 0);
    assert(cfg.autosave_interval_sec == 45);
    assert(strcmp(cfg.llm_host, "10.0.0.5") == 0);
    assert(cfg.llm_port == 9000);

    printf("All config_parser tests passed.\n");
    return 0;
}
