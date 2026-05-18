/* =========================================================
   CSS (STYLE GLOBAL)
   ========================================================= */
static void setup_css(const char *theme) {

    GtkCssProvider *provider = gtk_css_provider_new();
    gboolean dark = theme && g_ascii_strcasecmp(theme, "dark") == 0;

    const gchar *window_bg = dark ? "#2b2b2b" : "#e8e8e8";
    const gchar *window_fg = dark ? "#e8e8e8" : "#1e1e1e";
    const gchar *toolbar_bg = dark ? "#333333" : "#f7f7f7";
    const gchar *page_bg = dark ? "#1e1e1e" : "#ffffff";
    const gchar *page_fg = dark ? "#f0f0f0" : "#1e1e1e";
    const gchar *status_bg = dark ? "#2f2f2f" : "#f0f0f0";
    const gchar *status_fg = dark ? "#bbbbbb" : "#666666";

    gchar *css = g_strdup_printf(
        /* ===== FENÊTRE ===== */
        "window#app-window {"
        "    background-color: %s;"
        "    color: %s;"
        "}"

        /* ===== TOOLBAR ===== */
        "#toolbar {"
        "    background-color: %s;"
        "    border-bottom: 1px solid #d0d0d0;"
        "    padding: 8px 12px;"
        "}"
        
        "button#toolbar-button {"
        "    background-color: %s;"
        "    color: %s;"
        "    border: 1px solid #bfbfbf;"
        "    border-radius: 3px;"
        "    padding: 6px 12px;"
        "    font-size: 13px;"
        "}"
        
        "button#toolbar-button:hover {"
        "    background-color: #e8e8e8;"
        "    border: 1px solid #9e9e9e;"
        "}"

        /* ===== DOCUMENT CONTAINER (feuille) ===== */
        "#a4-page {"
        "    background-color: %s;"
        "    border: 1px solid #cfcfcf;"
        "    box-shadow: 0 10px 30px rgba(0,0,0,0.15);"
        "}"

        /* ===== ZONE D'ÉDITION (FEUILLE) ===== */
        "#editor-textview {"
        "    background-color: %s;"
        "    color: %s;"
        "    padding: 0px;"
        "    border: none;"
        "    font-family: 'Calibri', 'Arial', sans-serif;"
        "    font-size: 14px;"
        "}"

        "textview text {"
        "    background-color: %s;"
        "    color: %s;"
        "    caret-color: #111111;"
        "}"

        "#horizontal-ruler {"
        "    background-color: #f3f3f3;"
        "    border-bottom: 1px solid #cdcdcd;"
        "}"

        "#vertical-ruler {"
        "    background-color: #f3f3f3;"
        "    border-right: 1px solid #cdcdcd;"
        "}"

        /* ===== STATUSBAR ===== */
        "#statusbar {"
        "    background-color: %s;"
        "    color: %s;"
        "    border-top: 1px solid #d0d0d0;"
        "    padding: 4px 8px;"
        "    font-size: 12px;"
        "}"

        /* ===== SIDEBAR ===== */
        "#sidebar {"
        "    background-color: #f9f9f9;"
        "    border-left: 1px solid #eaeaea;"
        "    padding: 12px;"
        "}"

        /* ===== BOUTONS SIMPLES (STYLE ÉDITEUR) ===== */
        "button {"
        "    background-color: #ffffff;"
        "    color: #6f51bb;"
        "    border: 1px solid #dcdcdc;"
        "    border-radius: 6px;"
        "    padding: 6px 12px;"
        "}"

        "button:hover {"
        "    background-color: #f0f0f0;"
        "}"

        "button:active {"
        "    background-color: #e6e6e6;"
        "}"

        /* ===== STATUS BAR ACCENT ===== */
        "statusbar#statusbar {"
        "    background-color: %s;"
        "    color: %s;"
        "    border-top: 1px solid #e6e6e6;"
        "    padding: 6px 10px;"
        "    font-size: 12px;"
        "}"

        "#horizontal-ruler {"
        "   background-color: #f8fafc;"
        "   border-bottom: 1px solid #cbd5e1;"
        "}"

        "#vertical-ruler {"
        "   background-color: #f8fafc;"
        "   border-right: 1px solid #cbd5e1;"
        "}"

        /* ===== FORMAT TOOLBAR ===== */
        "#format-toolbar {"
        "   background-color: #f8fafc;"
        "   border-bottom: 1px solid #cbd5e1;"
        "   padding: 6px;"
        "}"

        "#format-button {"
        "   background-color: white;"
        "   border: 1px solid #d1d5db;"
        "   border-radius: 4px;"
        "   min-width: 32px;"
        "   min-height: 32px;"
        "}"

        "#format-button:hover {"
        "   background-color: #e2e8f0;"
        "}",
        window_bg, window_fg,
        toolbar_bg, toolbar_bg, page_fg,
        page_bg, page_bg, page_fg,
        page_bg, page_fg,
        status_bg, status_fg,
        status_bg, status_fg
    );

    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    g_free(css);

    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
}