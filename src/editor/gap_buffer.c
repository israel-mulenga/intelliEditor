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
    gb->gap_end = 1024;

    return gb;
}

void gap_buffer_move_cursor(GapBuffer *gb, size_t new_position) {
    size_t current_text_length = gb->size - (gb->gap_end - gb->gap_start);

    if (new_position > current_text_length) {
        fprintf(stderr, "Error : Position overflow\n");
        return;
    }

    size_t distance = abs(gb->gap_start - new_position);

    if (new_position < gb->gap_start) {
        // MOUVEMENT VERS LA GAUCHE
        memmove(&gb->buffer[gb->gap_end - distance], &gb->buffer[new_position], distance);
        
        gb->gap_start -= distance;
        gb->gap_end -= distance;
    } 
    else if (new_position > gb->gap_start) {
        // MOUVEMENT VERS LA DROITE
        memmove(&gb->buffer[gb->gap_start], &gb->buffer[gb->gap_end], distance);
        
        gb->gap_start += distance;
        gb->gap_end += distance;
    }
}

void gap_buffer_insert(GapBuffer *gb, char c){
    if(gb->gap_start == gb->gap_end){
        // TODO Implement RESIZE
        
    }
    gb->buffer[gb->gap_start] = c;
    gb->gap_start++;
}
