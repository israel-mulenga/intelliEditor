cmake_minimum_required(VERSION 3.10)
project(IntelliEditor C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)

# Recherche de GTK3
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Inclusion des dossiers
include_directories(include ${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

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