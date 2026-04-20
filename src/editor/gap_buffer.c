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

    if (new_position > current_text_length) {
        fprintf(stderr, "Error : Position overflow\n");
        return;
    }


    if (new_position < gb->gap_start) {
        // MOUVEMENT VERS LA GAUCHE
        size_t distance = gb->gap_start - new_position;
        memmove(&gb->buffer[gb->gap_end - distance], &gb->buffer[new_position], distance);
        
        gb->gap_start -= distance;
        gb->gap_end -= distance;
    } 
    else if (new_position > gb->gap_start) {
        // MOUVEMENT VERS LA DROITE
        size_t distance = new_position - gb->gap_start;
        memmove(&gb->buffer[gb->gap_start], &gb->buffer[gb->gap_end], distance);
        
        gb->gap_start += distance;
        gb->gap_end += distance;
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
