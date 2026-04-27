#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

int get_min_words_from_json(const char *filename) {
// 1. Lire le fichier
	 if (filename == NULL) return -1;
	 FILE *f = fopen(filename, "r");
	 if (f == NULL) return -1;
	 fseek(f, 0, SEEK_END);
     long len = ftell(f);
	 if (len < 0) {
	 	fclose(f);
	 	return -1;
	 }
	 fseek(f, 0, SEEK_SET);
	 char *data = malloc(len + 1);
	 if (data == NULL) {
	 	fclose(f);
	 	return -1;
	 }
	 fread(data, 1, len, f);
	 fclose(f);
	 data[len] = '\0';

 // 2. Parser en mémoire
	cJSON *json = cJSON_Parse(data);
	if (json == NULL) {
		free(data);
		return -1;
	}

// 3. Extraire la valeur
	cJSON *min_words = cJSON_GetObjectItemCaseSensitive(json, "min_words");
	if (!cJSON_IsNumber(min_words)) {
		cJSON_Delete(json);
		free(data);
		return -1;
	}
	int value = min_words->valueint;

// 4. Nettoyer
	cJSON_Delete(json);
	free(data);

	return value;
	}
