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
    gb->gap_end = 0;

    return gb;
}


