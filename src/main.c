#include <gtk/gtk.h>
#include "config/config_parser.h"
#include "llm/llm_client.h"
#include "ui/window.h"
#include "editor/gap_buffer.h"

typedef struct {
    GapBuffer *gb;
    AppConfig config;
} AppStartupData;

static const char *resolve_config_path(void) {
    if (g_file_test("config.ini", G_FILE_TEST_EXISTS)) {
        return "config.ini";
    }
    return NULL;
}

static void apply_llm_config(const AppConfig *app_cfg) {
    LlmConfig llm_cfg;
    llm_config_default(&llm_cfg);
    g_strlcpy(llm_cfg.host, app_cfg->llm_host, sizeof(llm_cfg.host));
    llm_cfg.port = app_cfg->llm_port;
    llm_client_init(&llm_cfg);
}

int main(int argc, char *argv[]) {
    GtkApplication *app;
    int status;
    AppStartupData *startup = g_new0(AppStartupData, 1);

    startup->gb = gap_buffer_create(1024);
    app_config_set_defaults(&startup->config);

    const char *config_path = resolve_config_path();
    if (config_path) {
        if (!app_config_load(config_path, &startup->config)) {
            g_printerr("Warning: could not parse %s, using defaults.\n", config_path);
            app_config_set_defaults(&startup->config);
        }
    }

    apply_llm_config(&startup->config);

    app = gtk_application_new("com.example.intellieditor", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(create_main_window), startup);
    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    llm_client_cleanup();
    g_free(startup);

    return status;
}
