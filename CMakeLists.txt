cmake_minimum_required(VERSION 3.10)
project(IntelliEditor C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(GTKSOURCEVIEW REQUIRED gtksourceview-4)
pkg_check_modules(HUNSPELL REQUIRED hunspell)
pkg_check_modules(CURL REQUIRED libcurl)
pkg_check_modules(PCRE2 REQUIRED libpcre2-8)
pkg_check_modules(GLIB REQUIRED glib-2.0)

function(intellieditor_apply_warnings target)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wpedantic
        )
    endif()
endfunction()

# ---------------------------------------------------------------------------
# Shared libraries
# ---------------------------------------------------------------------------

add_library(rules_engine STATIC
    src/rules/checkers.c
    src/rules/parser.c
    deps/cjson/cJSON.c
)
target_include_directories(rules_engine PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/deps/cjson
)
target_include_directories(rules_engine PRIVATE ${PCRE2_INCLUDE_DIRS})
target_link_libraries(rules_engine PUBLIC m ${PCRE2_LIBRARIES})
intellieditor_apply_warnings(rules_engine)

add_library(llm_client STATIC
    src/llm/llm_client.c
    deps/cjson/cJSON.c
)
target_include_directories(llm_client PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/deps/cjson
)
target_include_directories(llm_client PRIVATE
    ${CURL_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
)
target_link_libraries(llm_client PUBLIC ${CURL_LIBRARIES} ${GLIB_LIBRARIES})
target_compile_options(llm_client PRIVATE ${CURL_CFLAGS_OTHER} ${GLIB_CFLAGS_OTHER})
intellieditor_apply_warnings(llm_client)

add_library(editor_core STATIC
    src/editor/gap_buffer.c
    src/editor/file_manager.c
    src/config/config_parser.c
)
target_include_directories(editor_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
target_include_directories(editor_core PRIVATE ${GLIB_INCLUDE_DIRS})
target_link_libraries(editor_core PUBLIC ${GLIB_LIBRARIES})
target_compile_options(editor_core PRIVATE ${GLIB_CFLAGS_OTHER})
intellieditor_apply_warnings(editor_core)

# ---------------------------------------------------------------------------
# Application
# ---------------------------------------------------------------------------

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
    rules_engine
    llm_client
    ${GTK3_LIBRARIES}
    ${GTKSOURCEVIEW_LIBRARIES}
    ${HUNSPELL_LIBRARIES}
)
intellieditor_apply_warnings(IntelliEditor)

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------

enable_testing()

add_executable(test_gap_buffer
    tests/test_gap_buffer.c
)
target_include_directories(test_gap_buffer PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test_gap_buffer PRIVATE editor_core)
intellieditor_apply_warnings(test_gap_buffer)
add_test(NAME gap_buffer COMMAND test_gap_buffer)

add_executable(test_config_parser
    tests/test_config_parser.c
)
target_include_directories(test_config_parser PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test_config_parser PRIVATE editor_core)
intellieditor_apply_warnings(test_config_parser)
add_test(NAME config_parser COMMAND test_config_parser)

add_executable(test_llm
    tests/test_llm.c
)
target_include_directories(test_llm PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test_llm PRIVATE llm_client ${GTK3_LIBRARIES})
target_include_directories(test_llm PRIVATE ${GTK3_INCLUDE_DIRS})
intellieditor_apply_warnings(test_llm)
add_test(NAME llm COMMAND test_llm)
set_tests_properties(llm PROPERTIES TIMEOUT 120)
