cmake_minimum_required(VERSION 3.10)
project(IntelliEditor C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(GTKSOURCEVIEW REQUIRED gtksourceview-4)
pkg_check_modules(HUNSPELL REQUIRED hunspell)
pkg_check_modules(CURL REQUIRED libcurl)
pkg_check_modules(PCRE2 REQUIRED libpcre2-8) 

include_directories(
    include
    src
    deps/cjson
    ${GTK3_INCLUDE_DIRS}
    ${GTKSOURCEVIEW_INCLUDE_DIRS}
    ${HUNSPELL_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
    ${PCRE2_INCLUDE_DIRS} 
)

link_directories(
    ${GTK3_LIBRARY_DIRS}
    ${GTKSOURCEVIEW_LIBRARY_DIRS}
    ${HUNSPELL_LIBRARY_DIRS}
    ${CURL_LIBRARY_DIRS}
    ${PCRE2_LIBRARY_DIRS} 
)

add_definitions(
    ${GTK3_CFLAGS_OTHER}
    ${GTKSOURCEVIEW_CFLAGS_OTHER}
    ${HUNSPELL_CFLAGS_OTHER}
    ${CURL_CFLAGS_OTHER}
    ${PCRE2_CFLAGS_OTHER} 
)

# 1. ON DÉFINIT D'ABORD LA BIBLIOTHÈQUE RULES_ENGINE
add_library(rules_engine STATIC
    src/rules/checkers.c
    src/rules/parser.c
    deps/cjson/cJSON.c
)

target_include_directories(rules_engine PUBLIC
    include
    deps/cjson
)

target_link_libraries(rules_engine m ${PCRE2_LIBRARIES})

# 2. ON DÉFINIT ENSUITE L'EXÉCUTABLE PRINCIPAL
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

# 3. ON PEUT MAINTENANT FAIRE LE LIEN SANS ERREUR
target_link_libraries(IntelliEditor
    ${GTK3_LIBRARIES}
    ${GTKSOURCEVIEW_LIBRARIES}
    ${HUNSPELL_LIBRARIES}
    ${CURL_LIBRARIES}
    rules_engine  
)

# Cible de test indépendante
add_executable(test_gap_buffer
    tests/test_gap_buffer.c
    src/editor/gap_buffer.c
)
# Cible de test pour le moteur de règles
add_executable(test_rules
    tests/test_rules.c
)
target_link_libraries(test_rules rules_engine)