# IntelliEditor

Logiciel de traitement de texte intelligent en C (Projet L3 GL / UDBL 25-26).

## Prérequis

Le projet nécessite les bibliothèques suivantes :
* GTK 3.0
* Hunspell
* cJSON
* PCRE2

## Installation rapide (Linux)

Pour installer toutes les dépendances d'un coup, utilisez le script de configuration :
```bash
chmod +x setup_deps.sh
./setup_deps.sh
```

## Compilation et Exécution

Le projet utilise **CMake** pour gérer la compilation de manière portable. Suivez ces étapes pour builder le projet :

1. **Créer le dossier de build** (pour isoler les fichiers compilés du code source) :
   ```bash
   mkdir -p build && cd build
   ```