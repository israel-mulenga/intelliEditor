cmake_minimum_required(VERSION 3.10)
project(IntelliEditor C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)

# Recherche des bibliothèques nécessaires (GTK3 pour Linux)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Inclusion des dossiers
include_directories(include ${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

# Définition de l'exécutable
add_executable(IntelliEditor src/main.c)

# Liaison avec GTK3
target_link_libraries(IntelliEditor ${GTK3_LIBRARIES})