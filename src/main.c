#include <stdio.h>
#include <stdlib.h>
#include "rules.h"
#include "cJSON.h"

// Prototype de ta fonction de parsing (définie dans parser.c)
int get_min_words_from_json(const char *filename);

// Prototype de ta fonction de vérification (définie dans checkers.c)
RuleStatus check_word_count_min(const char *text, int min_limit);

int main() {
	printf("=== Test du Module de Regles (Semaine 1 & 2) ===\n\n");

	// 1. Test du Parsing JSON (Objectif Semaine 1)
	const char *config_path = "src/config.json";
	int min_required = get_min_words_from_json(config_path);

	if (min_required == -1) {
		printf("[ERREUR] Impossible de lire ou parser %s\n", config_path);
		return EXIT_FAILURE;
	                        }
	printf("[INFO] Seuil minimum recupere du JSON : %d mots.\n", min_required);

	// 2. Test de la Logique de Regle (Objectif Semaine 2 - Avance)
	const char *user_text = "Bonjour, ceci est un test pour le projet IntelliEditor a l'UDBL.";
	printf("[TEST] Analyse du texte : \"%s\"\n", user_text);

	RuleStatus result = check_word_count_min(user_text, min_required);

	// 3. Affichage du resultat
	if (result == STATUS_OK) {
	printf("\n✅ RESULTAT : Le texte respecte la regle (%d mots minimum).\n", min_required);
	} else {
	  	printf("\n❌ RESULTAT : Le texte est trop court (Moins de %d mots).\n", min_required);
	       }

		return EXIT_SUCCESS;
	}
