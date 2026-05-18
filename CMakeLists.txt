cmake_minimum_required(VERSION 3.10)

project(IntelliEditor C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic -D_GNU_SOURCE)

# ── Détection des bibliothèques ──────────────────────────────────
find_package(PkgConfig REQUIRED)

pkg_check_modules(GTK3          REQUIRED gtk+-3.0)
pkg_check_modules(GTKSOURCEVIEW REQUIRED gtksourceview-4)
pkg_check_modules(HUNSPELL      REQUIRED hunspell)
pkg_check_modules(CURL          REQUIRED libcurl)
pkg_check_modules(PCRE2         REQUIRED libpcre2-8)
pkg_check_modules(GLIB          REQUIRED glib-2.0)

find_package(Threads REQUIRED)

# ── Bibliothèque Rules (DEV-D) ───────────────────────────────────
add_library(rules_engine STATIC
    src/rules/checkers.c
    src/rules/parser.c
    src/rules/ruleset_eval.c
    deps/cjson/cJSON.c
)

target_include_directories(rules_engine PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/deps/cjson
)
target_include_directories(rules_engine PRIVATE ${PCRE2_INCLUDE_DIRS})
target_link_libraries(rules_engine PUBLIC m ${PCRE2_LIBRARIES})

# ── Bibliothèque LLM (DEV-C) ─────────────────────────────────────
set(LLM_ENGINE_SOURCES
    src/llm/llm_client.c
    deps/cjson/cJSON.c
)

foreach(_llm_optional llm_bridge.c llm_status.c)
    set(_llm_path src/llm/${_llm_optional})
    if(EXISTS "${CMAKE_SOURCE_DIR}/${_llm_path}")
        list(APPEND LLM_ENGINE_SOURCES ${_llm_path})
    endif()
endforeach()

add_library(llm_engine STATIC ${LLM_ENGINE_SOURCES})
target_include_directories(llm_engine PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/deps/cjson
)
target_include_directories(llm_engine PRIVATE
    ${CURL_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    ${GTK3_INCLUDE_DIRS}
    ${GTKSOURCEVIEW_INCLUDE_DIRS}
)
target_link_libraries(llm_engine PUBLIC
    ${GTK3_LIBRARIES}
    ${GTKSOURCEVIEW_LIBRARIES}
)
target_link_libraries(llm_engine PUBLIC
    ${CURL_LIBRARIES}
    ${GLIB_LIBRARIES}
    Threads::Threads
)

# ── Bibliothèque NLP (DEV-C) ─────────────────────────────────────
set(NLP_ENGINE_SOURCES
    src/nlp/hunspell_wrap.c
    src/nlp/nlp_engine.c
    src/nlp/nlp_highlighter.c
)

set(NLP_ENGINE_READY TRUE)
foreach(_nlp_src ${NLP_ENGINE_SOURCES})
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/${_nlp_src}")
        set(NLP_ENGINE_READY FALSE)
    endif()
endforeach()

if(NLP_ENGINE_READY)
    add_library(nlp_engine STATIC ${NLP_ENGINE_SOURCES})
    target_include_directories(nlp_engine PUBLIC
        ${CMAKE_SOURCE_DIR}/include
    )
    target_include_directories(nlp_engine PRIVATE
        ${GTK3_INCLUDE_DIRS}
        ${GTKSOURCEVIEW_INCLUDE_DIRS}
        ${HUNSPELL_INCLUDE_DIRS}
    )
    target_link_libraries(nlp_engine
        ${HUNSPELL_LIBRARIES}
        ${GTK3_LIBRARIES}
        ${GTKSOURCEVIEW_LIBRARIES}
    )
endif()

# ── Core éditeur + config (DEV-A) ────────────────────────────────
add_library(editor_core STATIC
    src/editor/gap_buffer.c
    src/editor/file_manager.c
    src/config/config_parser.c
)

target_include_directories(editor_core PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(editor_core PRIVATE ${GLIB_INCLUDE_DIRS})
target_link_libraries(editor_core PUBLIC ${GLIB_LIBRARIES})

# ── Exécutable principal ─────────────────────────────────────────
add_executable(IntelliEditor
    src/main.c
    src/ui/window.c
    src/ui/editor.c
    src/ui/sidebar.c
    src/ui/toolbar.c
    src/ui/callbacks.c
)

target_include_directories(IntelliEditor PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${GTK3_INCLUDE_DIRS}
    ${GTKSOURCEVIEW_INCLUDE_DIRS}
    ${HUNSPELL_INCLUDE_DIRS}
)
target_compile_options(IntelliEditor PRIVATE
    ${GTK3_CFLAGS_OTHER}
    ${GTKSOURCEVIEW_CFLAGS_OTHER}
    ${HUNSPELL_CFLAGS_OTHER}
)
target_link_libraries(IntelliEditor PRIVATE
    editor_core
    llm_engine
    rules_engine
    ${GTK3_LIBRARIES}
    ${GTKSOURCEVIEW_LIBRARIES}
    ${HUNSPELL_LIBRARIES}
    Threads::Threads
    m
)

if(NLP_ENGINE_READY)
    target_link_libraries(IntelliEditor PRIVATE nlp_engine)
endif()

# ── Tests ────────────────────────────────────────────────────────
enable_testing()

add_executable(test_gap_buffer tests/test_gap_buffer.c)
target_link_libraries(test_gap_buffer PRIVATE editor_core)
add_test(NAME test_gap_buffer COMMAND test_gap_buffer)

add_executable(test_config_parser tests/test_config_parser.c)
target_link_libraries(test_config_parser PRIVATE editor_core)
add_test(NAME test_config_parser COMMAND test_config_parser)

add_executable(test_rules tests/test_rules.c)
target_link_libraries(test_rules PRIVATE rules_engine)
add_test(NAME test_rules COMMAND test_rules)
set_tests_properties(test_rules PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_executable(test_llm tests/test_llm.c)
target_include_directories(test_llm PRIVATE ${GTK3_INCLUDE_DIRS})
target_link_libraries(test_llm PRIVATE llm_engine ${GTK3_LIBRARIES} Threads::Threads)
add_test(NAME test_llm COMMAND test_llm)
set_tests_properties(test_llm PROPERTIES TIMEOUT 120)
