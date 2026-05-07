#include <stdlib.h>
#include "editor/gap_buffer.h"

GapBuffer* gap_buffer_create(size_t initial_capacity) {
    GapBuffer *gb = (GapBuffer *)malloc(sizeof *gb);
    if (!gb) return NULL;

    gb->buffer = (char *)malloc(initial_capacity);
    if (!gb->buffer) {
        free(gb);
        return NULL;
    }

    gb->size = initial_capacity;
    gb->gap_start = 0;
    gb->gap_end = initial_capacity;
    gb->history = NULL;
    gb->redo = NULL;

    return gb;
}



void gap_buffer_move_cursor(GapBuffer *gb, size_t new_position) {
    size_t current_text_length = gb->size - (gb->gap_end - gb->gap_start);
    if (new_position > current_text_length) new_position = current_text_length;

    while(gb->gap_start > new_position) {
        gb->gap_start--;
        gb->gap_end--;
        gb->buffer[gb->gap_end] = gb->buffer[gb->gap_start];
    }

    while(gb->gap_start < new_position){
        gb->buffer[gb->gap_start] = gb->buffer[gb->gap_end];
        gb->gap_start++;
        gb->gap_end++;
    }
}

void gap_buffer_destroy(GapBuffer *gb){
    free(gb->buffer);
    // Libère toute la pile d'historique undo
    HistoryNode *node = gb->history;
    while (node) {
        HistoryNode *next = node->next;
        free(node->buffer); // Libère la copie du buffer
        free(node);         // Libère le nœud
        node = next;
    }
    // Libère toute la pile d'historique redo
    node = gb->redo;
    while (node) {
        HistoryNode *next = node->next;
        free(node->buffer);
        free(node);
        node = next;
    }
    free(gb);
}

GapBuffer* gap_buffer_resize(GapBuffer *gb) {
    size_t old_size = gb->size;
    size_t new_size = old_size * 2;
    
    char *new_buffer = (char *)malloc(new_size);
    if (!new_buffer) return NULL; 

    size_t prefix_len = gb->gap_start; 
    size_t suffix_len = old_size - gb->gap_end;
    memcpy(new_buffer, gb->buffer, prefix_len);


    size_t new_gap_end = new_size - suffix_len;
    memcpy(new_buffer + new_gap_end, gb->buffer + gb->gap_end, suffix_len);

    free(gb->buffer);
    gb->buffer = new_buffer;
    gb->size = new_size;
    gb->gap_end = new_gap_end;

    return gb;
}


void gap_buffer_insert(GapBuffer *gb, char c){
    gap_buffer_push_history(gb); // Sauvegarde l'état avant insertion pour permettre undo
    if(gb->gap_start == gb->gap_end){
        GapBuffer *resized_gb = gap_buffer_resize(gb);
        if(!resized_gb){
            fprintf(stderr, "Out of memory: insertion failed\n");
            return;
        }
                }
    gb->buffer[gb->gap_start] = c;
    gb->gap_start++;
}


char* gap_buffer_get_content(GapBuffer *gb) {
    size_t prefix_len = gb->gap_start;
    size_t suffix_len = gb->size - gb->gap_end;
    size_t text_length = prefix_len + suffix_len;

    char *text = (char *)malloc(text_length + 1);
    if (!text) return NULL;

    memcpy(text, gb->buffer, prefix_len);
    memcpy(text + prefix_len, gb->buffer + gb->gap_end, suffix_len);
    text[text_length] = '\0';

    return text;
}

void gap_buffer_backspace(GapBuffer *gb) {
    gap_buffer_push_history(gb); // Sauvegarde l'état avant suppression pour permettre undo
    if (gb->gap_start > 0) {
        gb->gap_start--;
    }
}

void gap_buffer_delete(GapBuffer *gb) {
    gap_buffer_push_history(gb); // Sauvegarde l'état avant suppression pour permettre undo
    if (gb->gap_end < gb->size) {
        gb->gap_end++;
    }
}

CursorPos gap_buffer_get_cursor_pos(GapBuffer *gb) {
    CursorPos pos = {0, 0};
    for (size_t i = 0; i < gb->gap_start; i++) {
        if (gb->buffer[i] == '\n') {
            pos.y++;
            pos.x = 0;
        } else {
            pos.x++;
        }
    }
    return pos;
}

void gap_buffer_set_cursor_pos(GapBuffer *gb, int x, int y) {
    size_t current_x = 0;
    size_t current_y = 0;
    size_t logical_pos = 0;
    
    // On calcule la taille totale du texte (sans le gap)
    size_t total_text_len = gb->size - (gb->gap_end - gb->gap_start);

    // On parcourt le texte de manière "logique"
    while (logical_pos < total_text_len) {
        // Si on a atteint la bonne ligne et la bonne colonne
        if (current_y == (size_t)y && current_x == (size_t)x) {
            break;
        }

        // On récupère le caractère à la position logique
        // (Si l'index dépasse gap_start, on saute à gap_end)
        size_t physical_pos = (logical_pos < gb->gap_start) ? 
                               logical_pos : 
                               logical_pos + (gb->gap_end - gb->gap_start);
        
        char c = gb->buffer[physical_pos];

        if (c == '\n') {
            // Si on cherche une position sur cette ligne mais que x est trop grand,
            // on s'arrête à la fin de la ligne actuelle
            if (current_y == (size_t)y) {
                break; 
            }
            current_y++;
            current_x = 0;
        } else {
            current_x++;
        }

        logical_pos++;
        
        // Sécurité : si on a dépassé la ligne cible, on s'arrête
        if (current_y > (size_t)y) break;
    }

    // On déplace physiquement le gap à cette position logique
    gap_buffer_move_cursor(gb, logical_pos);
}

static void gap_buffer_clear_redo(GapBuffer *gb) {
    HistoryNode *node = gb->redo;
    while (node) {
        HistoryNode *next = node->next;
        free(node->buffer);
        free(node);
        node = next;
    }
    gb->redo = NULL;
}

static void gap_buffer_push_redo_history(GapBuffer *gb) {
    HistoryNode *node = (HistoryNode *)malloc(sizeof(HistoryNode));
    if (!node) return;

    node->buffer = (char *)malloc(gb->size);
    if (!node->buffer) {
        free(node);
        return;
    }
    memcpy(node->buffer, gb->buffer, gb->size);
    node->gap_start = gb->gap_start;
    node->gap_end = gb->gap_end;
    node->size = gb->size;
    node->next = gb->redo;
    gb->redo = node;
}

void gap_buffer_push_history(GapBuffer *gb) {
    // Toute nouvelle action invalide la pile redo
    gap_buffer_clear_redo(gb);

    // Crée un nouveau nœud d'historique undo
    HistoryNode *node = (HistoryNode *)malloc(sizeof(HistoryNode));
    if (!node) return; // Échec d'allocation

    // Copie le buffer actuel
    node->buffer = (char *)malloc(gb->size);
    if (!node->buffer) {
        free(node);
        return; // Échec d'allocation
    }
    memcpy(node->buffer, gb->buffer, gb->size);

    // Copie les autres champs
    node->gap_start = gb->gap_start;
    node->gap_end = gb->gap_end;
    node->size = gb->size;

    // Ajoute en tête de la pile undo
    node->next = gb->history;
    gb->history = node;
}

void gap_buffer_undo(GapBuffer *gb) {
    if (!gb->history) return; // Pas d'historique à restaurer

    // Sauvegarde l'état courant pour redo
    gap_buffer_push_redo_history(gb);

    // Récupère le dernier état sauvegardé dans undo
    HistoryNode *node = gb->history;
    gb->history = node->next;

    // Restaure le buffer
    free(gb->buffer);
    gb->buffer = node->buffer;
    gb->gap_start = node->gap_start;
    gb->gap_end = node->gap_end;
    gb->size = node->size;

    // Libère le nœud undo
    free(node);
}

void gap_buffer_redo(GapBuffer *gb) {
    if (!gb->redo) return; // Pas d'état redo disponible

    // Sauvegarde l'état courant dans undo
    HistoryNode *node = (HistoryNode *)malloc(sizeof(HistoryNode));
    if (!node) return;

    node->buffer = (char *)malloc(gb->size);
    if (!node->buffer) {
        free(node);
        return;
    }
    memcpy(node->buffer, gb->buffer, gb->size);
    node->gap_start = gb->gap_start;
    node->gap_end = gb->gap_end;
    node->size = gb->size;
    node->next = gb->history;
    gb->history = node;

    // Récupère l'état suivant dans redo
    HistoryNode *redo_node = gb->redo;
    gb->redo = redo_node->next;

    free(gb->buffer);
    gb->buffer = redo_node->buffer;
    gb->gap_start = redo_node->gap_start;
    gb->gap_end = redo_node->gap_end;
    gb->size = redo_node->size;

    free(redo_node);
}