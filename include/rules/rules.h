#ifndef RULES_H
#define RULES_H

/**
 * @brief RuleStatus : Définit l'état de conformité d'une règle.
 * Utile pour l'intégration avec l'IHM (DEV-B) pour l'affichage des icônes.
 */
typedef enum {
    STATUS_OK,       // La règle est respectée (Vert)
    STATUS_WARNING,  // Attention / Limite presque atteinte (Orange)
    STATUS_ERROR,    // La règle est violée ou non conforme (Rouge)
    STATUS_PENDING   // En attente d'analyse complémentaire
} RuleStatus;

/**
 * @brief Rule : Représente une règle unique extraite du fichier JSON.
 */
typedef struct {
    char id[10];            // Identifiant unique (ex: "R001")
    char category[20];      // Catégorie de la règle (structure, style, etc.)
    char description[256];  // Message d'explication pour l'utilisateur
    char check_type[50];    // Type de vérification (word_count, regex)
    char parameter[512];    // Paramètre associé (valeur min ou motif regex)
    RuleStatus status;      // État actuel de conformité de cette règle
} Rule;

/**
 * @brief RuleSet : Regroupe toutes les règles chargées pour un document.
 */
typedef struct {
    Rule *rules;            // Tableau dynamique de structures Rule
    int rule_count;         // Nombre total de règles chargées
    char doc_type[100];     // Type de document ciblé (ex: "Mémoire de Licence")
} RuleSet;

/**
 * @brief ConfigRules : Configuration globale simplifiée lue depuis le JSON.
 */
typedef struct {
    int min_words;             // Seuil minimum de mots requis
    char **forbidden_patterns; // Tableau dynamique de motifs (ex: ["secret", "hack"])
    int forbidden_count;       // Nombre total de motifs interdits enregistrés
} ConfigRules;

// ============================================================================
// PROTOTYPES DES FONCTIONS DU MOTEUR DE RÈGLES (DEV-D)
// ============================================================================

/* --- Gestion et Parsing de la Configuration --- */
ConfigRules* parse_config(const char *filename);
void free_config(ConfigRules *config);

/* --- Analyse par Expressions Régulières (PCRE2) --- */
int check_forbidden_regex(const char *text, const char *pattern);

/* --- Analyse du Volume de Texte (Comptage de mots) --- */
int count_words(const char *text);
int check_word_count_min(const char *text, int min_words);

#endif // RULES_H