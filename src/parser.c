#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

int get_min_words_from_json(const char *filename) {
// 1. Lire le fichier
	 FILE *f = fopen(filename, "r");
	 fseek(f, 0, SEEK_END);
     long len = ftell(f);
	 fseek(f, 0, SEEK_SET);
	 char *data = malloc(len + 1);
	 fread(data, 1, len, f);
	 fclose(f);
	 data[len] = '\0';

 // 2. Parser en mémoire
	cJSON *json = cJSON_Parse(data);
	if (json == NULL) return -1;

// 3. Extraire la valeur
	cJSON *min_words = cJSON_GetObjectItemCaseSensitive(json, "min_words");
	int value = min_words->valueint;

// 4. Nettoyer
	cJSON_Delete(json);
	free(data);

	return value;
	}
