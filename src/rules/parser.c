#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "rules/rules.h"

ConfigRules* parse_config(const char *filename) {
    // 1. Lecture classique du fichier
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erreur : Impossible d'ouvrir le fichier %s\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *data = malloc(length + 1);
    fread(data, 1, length, file);
    data[length] = '\0';
    fclose(file);

    // 2. Parsing du JSON
    cJSON *json = cJSON_Parse(data);
    free(data);
    if (!json) {
        printf("Erreur de parsing JSON.\n");
        return NULL;
    }

    ConfigRules *config = malloc(sizeof(ConfigRules));
    config->forbidden_patterns = NULL;
    config->forbidden_count = 0;

    // Récupération de min_words
    cJSON *min_words = cJSON_GetObjectItemCaseSensitive(json, "min_words");
    if (cJSON_IsNumber(min_words)) {
        config->min_words = min_words->valueint;
    } else {
        config->min_words = 0; // Valeur par défaut
    }

    // 3. Récupération du tableau de patterns interdits
    cJSON *patterns = cJSON_GetObjectItemCaseSensitive(json, "forbidden_patterns");
    if (cJSON_IsArray(patterns)) {
        config->forbidden_count = cJSON_GetArraySize(patterns);
        
        if (config->forbidden_count > 0) {
            // Allocation de la mémoire pour le tableau de pointeurs
            config->forbidden_patterns = malloc(config->forbidden_count * sizeof(char*));
            
            for (int i = 0; i < config->forbidden_count; i++) {
                cJSON *item = cJSON_GetArrayItem(patterns, i);
                if (cJSON_IsString(item)) {
                    // On duplique la chaîne en mémoire pour la conserver
                    config->forbidden_patterns[i] = strdup(item->valuestring);
                }
            }
        }
    }

    cJSON_Delete(json);
    return config;
}

// Fonction de nettoyage pour éviter les fuites de mémoire (très important en C !)
void free_config(ConfigRules *config) {
    if (!config) return;

    // 1. Libérer chaque chaîne de caractères du tableau
    if (config->forbidden_patterns) {
        for (int i = 0; i < config->forbidden_count; i++) {
            if (config->forbidden_patterns[i]) {
                free(config->forbidden_patterns[i]);
            }
        }
        // 2. Libérer le tableau de pointeurs lui-même
        free(config->forbidden_patterns);
    }

    // 3. Libérer la structure globale
    free(config);
}