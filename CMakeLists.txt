cmake_minimum_required(VERSION 3.10)
project(IntelliEditor C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)

# Recherche de GTK3
# Recherche de GTK3
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(GTKSOURCEVIEW REQUIRED gtksourceview-4)
pkg_check_modules(HUNSPELL REQUIRED hunspell)

# Inclusion des dossiers
include_directories(include ${GTK3_INCLUDE_DIRS} ${GTKSOURCEVIEW_INCLUDE_DIRS} ${HUNSPELL_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS} ${GTKSOURCEVIEW_LIBRARY_DIRS} ${HUNSPELL_LIBRARY_DIRS})

# Ajout des flags de compilation
add_definitions(${GTK3_CFLAGS_OTHER} ${GTKSOURCEVIEW_CFLAGS_OTHER} ${HUNSPELL_CFLAGS_OTHER})

# --- EXÉCUTABLE PRINCIPAL ---
add_executable(IntelliEditor src/main.c src/ui/window.c src/ui/editor.c src/ui/sidebar.c src/ui/toolbar.c src/ui/callbacks.c src/editor/gap_buffer.c src/editor/file_manager.c) et Hunspell
target_link_libraries(IntelliEditor ${GTK3_LIBRARIES} ${GTKSOURCEVIEW_LIBRARIES} ${HUNSPELL_LIBRARIES})
# --- EXÉCUTABLE PRINCIPAL ---
add_executable(IntelliEditor src/main.c src/editor/gap_buffer.c src/editor/file_manager.c)
target_link_libraries(IntelliEditor ${GTK3_LIBRARIES})

# --- EXÉCUTABLE DE TESTS ---
# On crée un binaire séparé pour les tests
add_executable(test_gap_buffer 
    tests/test_gap_buffer.c 
    src/editor/gap_buffer.c
)
# Pas besoin de GTK pour les tests unitaires du buffer, 
# sauf si ton gap_buffer.h inclut des types GTK.

