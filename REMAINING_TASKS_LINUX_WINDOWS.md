# IntelliEditor - Reste a faire par developpeur (Linux + cahier Windows)

Date: 2026-04-27

Objectif: finir le projet sous Linux (GTK3) sans perdre la logique du cahier Windows (Win32 + Scintilla), et garder une base de code propre, testable, et merge-friendly.

## 1) Etat actuel verifie (factuel)

- Build Linux OK avec GTK3/GtkSourceView/Hunspell/cJSON/libcurl.
- Gap Buffer + import/save/save as existants.
- UI GTK existe (fenetre, toolbar menu File, editeur, statusbar).
- Moteur LLM HTTP existe (libcurl + cJSON), mais pas relie a l UI produit.
- Moteur de regles tres partiel (word_count_min seulement).
- PCRE2 non integre dans CMake ni dans le moteur de regles.
- Sidebar non connectee au cycle complet de validation des regles.
- Tests surtout gap_buffer; pas de suite regles + UI + llm integree.

## 2) Alignement Linux vs cahier Windows (strategie propre)

- Linux garde GTK3 + GtkSourceView (pas Win32/Scintilla).
- Chaque exigence Windows est mappee a un equivalent Linux:
  - Win32 window -> GtkApplication/GtkWindow
  - Scintilla editor -> GtkSourceView
  - Win threads -> GThread
  - llama.cpp C API embarquee -> llama-server + client HTTP libcurl
- Resultat attendu: memes fonctionnalites metier, implementation differente.

## 3) Reste a faire exact par DEV

## DEV-A (Infrastructure, CMake, fichiers, qualite de base)

A faire:

1. Stabiliser CMake par cibles (pas global).
2. Ajouter PCRE2 a la detection et au linkage.
3. Lier explicitement la cible IntelliEditor avec libcurl et rules_engine.
4. Ajouter une cible test_llm dans CMake + execution via ctest.
5. Ajouter un mode warning strict local (-Wall -Wextra -Wpedantic) sans casser CI.
6. Ajouter parser INI de config (theme, autosave_interval, llm host/port).
7. Implementer autosave periodique (si chemin document connu).
8. Verifier robustesse I/O UTF-8 (fichiers vides, gros fichiers, erreurs disque).

Comment faire proprement:

- Faire des commits atomiques: un commit CMake, un commit config INI, un commit autosave.
- Ne pas melanger refactor + feature dans le meme commit.
- Ajouter un test par correction (au minimum test unitaire parser INI, test integration autosave).
- Conserver API publique stable dans include/.

Definition of done DEV-A:

- cmake configure/build/test passent sans intervention manuelle.
- IntelliEditor linke sans symbols manquants (rules + curl).
- config.ini lu et applique au demarrage.

## DEV-B (UI GTK, UX, interaction)

A faire:

1. Utiliser GtkSourceView reel (pas GtkTextView simple) pour activer numerotation des lignes.
2. Activer line numbers via API GtkSourceView.
3. Integrer sidebar dans layout final (editeur a gauche, regles a droite).
4. Rendre statusbar dynamique (mots, ligne, colonne, encodage).
5. Ajouter actions UI pour Correct et Reformuler reliees aux callbacks.
6. Ajouter raccourcis clavier (Ctrl+O, Ctrl+S, Ctrl+Shift+S, Ctrl+Q).
7. Ajouter mode multi-document simple (onglets GTK) ou document actif unique + verrouillage explicite (decision equipe).

Comment faire proprement:

- Garder toute creation de widgets dans ui/\*.c, pas dans main.c.
- Encapsuler chaque zone UI dans une fonction create\_\*.
- Eviter logique metier dans callbacks UI: appeler une API metier dediee.
- Eviter CSS dupliquee; garder un seul provider central.

Definition of done DEV-B:

- UI complete selon linux spec, stable, sans blocage de thread UI.
- Sidebar et statusbar refletent etat reel du document.

## DEV-C (LLM + NLP)

A faire:

1. Brancher on_rewrite_clicked sur llm_rephrase_async.
2. Ajouter callback thread-safe vers UI (g_idle_add) pour mise a jour sidebar/statusbar.
3. Integrer llm_grammar_async sur selection/phrase courante.
4. Implementer pipeline Hunspell temps reel (pas seulement bouton).
5. Ajouter gestion des erreurs LLM visible utilisateur (timeout, serveur down, JSON invalide).
6. Centraliser prompts (include/llm/llm_prompts.h) et versionner formats de reponse.
7. Ajouter tests integration LLM (mock HTTP local ou mode replay JSON).

Comment faire proprement:

- Interdit de manipuler widgets GTK depuis thread worker.
- Toute MAJ UI passe par main loop GTK.
- Toujours liberer buffers HTTP et cJSON sur tous les chemins d erreur.
- Garder une API llm_client petite et documentee.

Definition of done DEV-C:

- Reformulation et correction LLM visibles depuis UI, sans freeze.
- Hunspell temps reel fonctionnel avec retours utilisateur clairs.

## DEV-D (Regles, JSON, regex, integration)

A faire:

1. Etendre parser JSON pour charger RuleSet complet (meta + rules[]).
2. Ajouter prototypes publics du moteur dans include/rules/rules.h.
3. Implementer checkers manquants:
   - section_exists
   - section_order
   - word_count_max
   - regex_forbidden
   - regex_required
   - heading_format
   - llm_semantic (via API DEV-C)
4. Integrer PCRE2 dans checkers regex.
5. Produire RuleReport standard (OK/WARNING/ERROR/PENDING + message + position).
6. Connecter RuleReport a la sidebar UI (liste + clic -> position curseur).
7. Declencher reevaluation apres 2 secondes d inactivite (debounce).

Comment faire proprement:

- Une fonction par checker, signature uniforme.
- Pas de logique regex inline dans UI.
- Valider tous les champs JSON avant usage (null, type, range).
- Garder code de parsing separé du code d evaluation.

Definition of done DEV-D:

- Un fichier JSON de regles charge sans crash.
- Rapport de conformite complet, coherent, affichable en UI.

## 4) Regles de proprete de base de code (obligatoires)

1. Branches: dev-a, dev-b, dev-c, dev-d; aucune feature directe sur main.
2. PR obligatoire avec review croisee (au moins 1 approbation).
3. Commits atomiques + messages conventionnels (feat/fix/refactor/test/docs/chore).
4. Headers publics dans include/, implementation dans src/.
5. Pas de dependance circulaire entre modules.
6. Warnings traites avant merge.
7. Tout malloc/free trace dans le meme flux logique.
8. Tests ajoutes pour toute fonctionnalite metier nouvelle.
9. Merge gate: build + tests + revue = obligatoire.
10. Release hebdo: tag + changelog court.

## 5) Priorites de livraison (ordre recommande, 2 semaines)

Semaine 1 (stabilite):

1. DEV-A: CMake cible + PCRE2 + tests ctest.
2. DEV-B: GtkSourceView line numbers + sidebar layout final.
3. DEV-C: callback LLM thread-safe + rewrite connecte.
4. DEV-D: parser RuleSet complet + checkers section_exists/word_count_min/max.

Semaine 2 (fonctionnel complet):

1. DEV-D: regex_forbidden/required + section_order + report.
2. DEV-C: grammar async + hunspell temps reel.
3. DEV-B: statusbar dynamique + navigation erreur.
4. DEV-A: autosave + config.ini final + CI build/test.

## 6) Definition of Done globale projet

Le projet est considere fini si:

1. Build propre sur Linux en une commande.
2. Edition + import/export + autosave fonctionnent.
3. Hunspell + LLM reformulation fonctionnent sans bloquer UI.
4. Regles JSON (structure, longueur, regex, semantic) produisent un rapport exploitable.
5. Sidebar montre les statuts et permet navigation.
6. Tests minimaux (core + rules + llm) passent en CI.
7. README explique installation, run, tests, architecture Linux.
