#include "editor/gap_buffer.h"

GapBuffer* gap_buffer_create(size_t initial_capacity) {
    GapBuffer *gb = malloc(sizeof(GapBuffer));
    if (!gb) return NULL;

    gb->buffer = malloc(initial_capacity);
    if (!gb->buffer) {
        free(gb);
        return NULL;
    }

    gb->size = initial_capacity;
    gb->gap_start = 0;
    gb->gap_end = initial_capacity;

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
    free(gb);
}

GapBuffer* gap_buffer_resize(GapBuffer *gb) {
    size_t old_size = gb->size;
    size_t new_size = old_size * 2;
    
    char *new_buffer = malloc(new_size);
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

    char *text = malloc(text_length + 1);
    if (!text) return NULL;

    memcpy(text, gb->buffer, prefix_len);
    memcpy(text + prefix_len, gb->buffer + gb->gap_end, suffix_len);
    text[text_length] = '\0';

    return text;
}

void gap_buffer_backspace(GapBuffer *gb) {
    if (gb->gap_start > 0) {
        gb->gap_start--;
    }
}

void gap_buffer_delete(GapBuffer *gb) {
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