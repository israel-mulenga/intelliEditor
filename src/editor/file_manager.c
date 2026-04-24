#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "editor/gap_buffer.h"

GapBuffer* gap_buffer_load_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return NULL;

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    GapBuffer *gb = gap_buffer_create((size_t)file_size + 64); 
    if (!gb) {
        fclose(file);
        return NULL;
    }

    size_t read_size = fread(gb->buffer, 1, (size_t)file_size, file);
    fclose(file);

    if (read_size != (size_t)file_size) {
        gap_buffer_destroy(gb);
        return NULL;
    }

    gb->gap_start = read_size; 
    gb->gap_end = gb->size; 

    return gb;
}

void gap_buffer_save_to_file(GapBuffer *gb, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) return;

    if (gb->gap_start > 0) {
        fwrite(gb->buffer, 1, gb->gap_start, file);
    }

    size_t suffix_len = gb->size - gb->gap_end;
    if (suffix_len > 0) {
        fwrite(gb->buffer + gb->gap_end, 1, suffix_len, file);
    }

    fclose(file);
}