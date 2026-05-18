#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rules/rules.h"

int main(void) {
    const char *path = rules_default_file_path();
    if (!path) {
        fprintf(stderr, "memoire_licence.json not found\n");
        return 1;
    }

    RuleSet *ruleset = ruleset_load(path);
    if (!ruleset) {
        fprintf(stderr, "Failed to load ruleset from %s\n", path);
        return 1;
    }

    printf("=== RuleSet: %s ===\n\n", path);
    printf("Type document : %s\n", ruleset->meta.document_type);
    printf("Version       : %s\n", ruleset->meta.version);
    printf("Regles chargees : %d\n\n", ruleset->rule_count);

    for (int i = 0; i < ruleset->rule_count; i++) {
        const Rule *rule = &ruleset->rules[i];
        printf("  %s | %-18s | %s\n", rule->id, rule->check_type, rule->description);
    }

    char sample[8192];
    size_t offset = 0;
    offset += (size_t)snprintf(sample + offset, sizeof(sample) - offset,
        "Introduction\n"
        "Ce memoire presente notre travail de recherche sur l'edition intelligente.\n");
    for (int w = 0; w < 310 && offset < sizeof(sample) - 16; w++) {
        offset += (size_t)snprintf(sample + offset, sizeof(sample) - offset, "mot%d ", w);
    }
    snprintf(sample + offset, sizeof(sample) - offset,
        "\nConclusion\nBibliographie\n");

    ruleset_evaluate(ruleset, sample);

    printf("\n=== Evaluation ===\n");
    int ok = ruleset_count_by_status(ruleset, STATUS_OK);
    int err = ruleset_count_by_status(ruleset, STATUS_ERROR);
    int warn = ruleset_count_by_status(ruleset, STATUS_WARNING);
    int pending = ruleset_count_by_status(ruleset, STATUS_PENDING);

    for (int i = 0; i < ruleset->rule_count; i++) {
        const Rule *rule = &ruleset->rules[i];
        printf("[%s] %s — %s\n",
               rule_status_icon(rule->status),
               rule->id,
               rule_status_label(rule->status));
    }

    printf("\nConformite : %d OK, %d erreur(s), %d avertissement(s), %d en attente\n",
           ok, err, warn, pending);

    if (ruleset->rule_count < 9) {
        ruleset_free(ruleset);
        return 1;
    }

    int result = (ok > 0) ? 0 : 1;
    ruleset_free(ruleset);
    return result;
}
