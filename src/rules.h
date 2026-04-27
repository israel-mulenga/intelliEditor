#ifndef RULES_H
#define RULES_H

/* * RuleStatus : Définit l'état de conformité d'une règle.
 * Utile pour le DEV-B qui affichera les icônes (Vert/Orange/Rouge).
  */
  typedef enum {
  	STATUS_OK,          // La règle est respectée
  	STATUS_WARNING,     // Attention (ex: limite de mots presque atteinte)
    STATUS_ERROR,       // La règle est violée (ex: section manquante)
    STATUS_PENDING      // En attente d'analyse (pour le LLM)
  	           } RuleStatus;

 /* * Rule : Représente une règle unique extraite du fichier JSON.
  */
  typedef struct {
 	 char id[10];          // Identifiant unique (ex: "R001")
  	 char category[20];    // Catégorie (structure, style, etc.)
  	 char description[256];// Message d'explication pour l'utilisateur
  	 char check_type[50];  // Type de vérification (word_count, regex)
  	 char parameter[512];  // Paramètre de la règle (valeur min ou motif regex)
  	 RuleStatus status;    // État actuel de cette règle
  	              } Rule;

 /* * RuleSet : Regroupe toutes les règles chargées pour un document.
  */
 typedef struct {
	 Rule *rules;          // Tableau dynamique de règles
  	 int rule_count;       // Nombre total de règles dans le fichier
     char doc_type[100];   // Type de document (ex: "Mémoire de Licence")
  	            } RuleSet;
#endif
  	                 	                         	            
  	                 	                        
  	                
  
