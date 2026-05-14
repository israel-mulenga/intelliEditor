# hi  bro finde here what i want you to add in the cmakelist file 

cmake_minimum_required(VERSION 3.10)
project(IntelliEditor C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Wextra -D_GNU_SOURCE)

# ── Détection des bibliothèques ──────────────────────────────────
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3         REQUIRED gtk+-3.0)
pkg_check_modules(GTKSOURCEVIEW REQUIRED gtksourceview-4)
pkg_check_modules(HUNSPELL     REQUIRED hunspell)
pkg_check_modules(CURL         REQUIRED libcurl)

# ── Includes globaux ─────────────────────────────────────────────
include_directories(
    include
    deps/cjson
    ${GTK3_INCLUDE_DIRS}
    ${GTKSOURCEVIEW_INCLUDE_DIRS}
    ${HUNSPELL_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
)

link_directories(
    ${GTK3_LIBRARY_DIRS}
    ${GTKSOURCEVIEW_LIBRARY_DIRS}
    ${HUNSPELL_LIBRARY_DIRS}
    ${CURL_LIBRARY_DIRS}
)

add_definitions(
    ${GTK3_CFLAGS_OTHER}
    ${GTKSOURCEVIEW_CFLAGS_OTHER}
    ${HUNSPELL_CFLAGS_OTHER}
    ${CURL_CFLAGS_OTHER}
)

# ── Bibliothèque NLP (DEV-C) ─────────────────────────────────────
add_library(nlp_engine STATIC
    src/nlp/hunspell_wrap.c
    src/nlp/nlp_engine.c
    src/nlp/nlp_highlighter.c
)
target_link_libraries(nlp_engine
    ${HUNSPELL_LIBRARIES}
    ${GTK3_LIBRARIES}
    ${GTKSOURCEVIEW_LIBRARIES}
)

# ── Bibliothèque LLM (DEV-C) ─────────────────────────────────────
add_library(llm_engine STATIC
    src/llm/llm_client.c
    src/llm/llm_bridge.c
    src/llm/llm_status.c
    deps/cjson/cJSON.c
)
target_link_libraries(llm_engine
    ${CURL_LIBRARIES}
    ${GTK3_LIBRARIES}
    ${GTKSOURCEVIEW_LIBRARIES}
    pthread
)

# ── Bibliothèque Rules (DEV-D) ───────────────────────────────────
add_library(rules_engine STATIC
    src/rules/checkers.c
    src/rules/parser.c
    deps/cjson/cJSON.c
)
target_include_directories(rules_engine PUBLIC include deps/cjson)
target_link_libraries(rules_engine m)

# ── Exécutable principal ─────────────────────────────────────────
add_executable(IntelliEditor
    src/main.c
    src/ui/window.c
    src/ui/editor.c
    src/ui/sidebar.c
    src/ui/toolbar.c
    src/ui/callbacks.c
    src/editor/gap_buffer.c
    src/editor/file_manager.c
)

target_link_libraries(IntelliEditor
    nlp_engine
    llm_engine
    rules_engine
    ${GTK3_LIBRARIES}
    ${GTKSOURCEVIEW_LIBRARIES}
    ${HUNSPELL_LIBRARIES}
    ${CURL_LIBRARIES}
    pthread
    m
)

# ── Tests ────────────────────────────────────────────────────────
add_executable(test_gap_buffer
    tests/test_gap_buffer.c
    src/editor/gap_buffer.c
)

add_executable(test_llm
    tests/test_llm.c
)
target_link_libraries(test_llm
    llm_engine
    ${GTK3_LIBRARIES}
    pthread
)

# ── CTest ────────────────────────────────────────────────────────
enable_testing()
add_test(NAME test_gap_buffer COMMAND test_gap_buffer)
add_test(NAME test_llm        COMMAND test_llm)