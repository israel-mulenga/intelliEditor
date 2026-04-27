#include <stdio.h>
#include <ctype.h>
#include "rules/rules.h"

/* * count_words : Parcourt le texte caractère par caractère.
 * Un mot est détecté quand on passe d'un espace à un caractère visible.
  * Cette fonction est robuste face aux espaces multiples.
   */
int count_words(const char *text) {
	if (text == NULL) return 0; // Sécurité : évite de crasher si le texte est vide

    		   int count = 0;
   	    	   int in_word = 0; // Drapeau (flag) : 1 si on est à l'intérieur d'un mot

   	           while (*text) {
   	           	if (isspace(*text)) {
   	            	in_word = 0; // On a trouvé un espace, on n'est plus dans un mot
   	                	        	} 
   	            else if (!in_word) {
   	            	in_word = 1; // On passe d'un espace à un caractère : nouveau mot trouvé
   	              	count++;
   	                	           }
   	                  text++; // On passe au caractère suivant (arithmétique de pointeurs)
   	                         }
   	            return count;
   	                              }

/* * check_word_count_min : Vérifie si le texte atteint le quota de mots.
* Renvoie STATUS_OK si conforme, sinon STATUS_ERROR.
*/
RuleStatus check_word_count_min(const char *text, int min_required) {
	int current_count = count_words(text);

 	if (current_count >= min_required) {
    	return STATUS_OK;
 	} else {
    	return STATUS_ERROR;
   	       } }
