#include <stdio.h>
#include <stdlib.h>
#include "rules/rules.h"

int main() {
    printf("=== BANC DE TEST DU MOTEUR DE REGLES (DEV-D) ===\n\n");

    // 1. Charger la configuration JSON
    ConfigRules *config = parse_config("config.json");
    if (!config) {
        printf("Erreur : Impossible de charger la configuration JSON.\n");
        return 1;
    }

    printf("Configuration chargee avec succes !\n");
    printf("- Seuil minimum de mots             : %d\n", config->min_words);
    printf("- Nombre de patterns regex interdits : %d\n\n", config->forbidden_count);

    // Faux texte d'éditeur pour simuler la saisie utilisateur
    const char *texte_utilisateur = "Ceci est un document hautement secret qui ne doit pas fuiter.";
    printf("Texte a analyser : \"%s\"\n\n", texte_utilisateur);

    // Variable globale de conformité pour le verdict
    int texte_conforme = 1;

    // 2. VÉRIFICATION DU NOMBRE DE MOTS MINIMUM
    printf("Verification du volume de texte... ");
    int mots_trouves = count_words(texte_utilisateur);
    
    if (check_word_count_min(texte_utilisateur, config->min_words)) {
        printf("[OK] Nombre de mots suffisant (%d/%d)\n", mots_trouves, config->min_words);
    } else {
        printf("[ALERTE] Pas assez de mots ! (%d/%d requis)\n", mots_trouves, config->min_words);
        texte_conforme = 0;
    }

    // 3. VÉRIFICATION DES EXPRESSIONS RÉGULIÈRES (MOTS INTERDITS)
    int violation_regex = 0;
    for (int i = 0; i < config->forbidden_count; i++) {
        printf("Verification du motif Regex : \"%s\"... ", config->forbidden_patterns[i]);
        
        if (check_forbidden_regex(texte_utilisateur, config->forbidden_patterns[i])) {
            printf("[ALERTE] Motif interdit detecte !\n");
            violation_regex = 1;
        } else {
            printf("[OK]\n");
        }
    }

    if (violation_regex) {
        texte_conforme = 0;
    }

    // 4. VERDICT FINAL DU MOTEUR DE RECHERCHE
    printf("\n--------------------------------------------------\n");
    if (texte_conforme) {
        printf("VERDICT FINAL : [SUCCES] Le texte respecte toutes les regles du document.\n");
    } else {
        printf("VERDICT FINAL : [ECHEC] Le texte viole une ou plusieurs regles metier.\n");
    }
    printf("--------------------------------------------------\n");

    // 5. Nettoyage complet de la mémoire RAM
    free_config(config);

    return 0;
}