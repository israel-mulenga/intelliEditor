cmake_minimum_required(VERSION 3.10)
project(IntelliEditor C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)

# Recherche des bibliothèques nécessaires (GTK3 pour Linux)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(GTKSOURCEVIEW REQUIRED gtksourceview-4)
pkg_check_modules(HUNSPELL REQUIRED hunspell)

# Inclusion des dossiers
include_directories(include ${GTK3_INCLUDE_DIRS} ${GTKSOURCEVIEW_INCLUDE_DIRS} ${HUNSPELL_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS} ${GTKSOURCEVIEW_LIBRARY_DIRS} ${HUNSPELL_LIBRARY_DIRS})

# Ajout des flags de compilation
add_definitions(${GTK3_CFLAGS_OTHER} ${GTKSOURCEVIEW_CFLAGS_OTHER} ${HUNSPELL_CFLAGS_OTHER})

# Définition de l'exécutable
add_executable(IntelliEditor src/main.c src/ui/window.c src/ui/editor.c src/ui/sidebar.c src/ui/toolbar.c src/ui/callbacks.c)

# Liaison avec GTK3 et Hunspell
target_link_libraries(IntelliEditor ${GTK3_LIBRARIES} ${GTKSOURCEVIEW_LIBRARIES} ${HUNSPELL_LIBRARIES})