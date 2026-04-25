#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "editor/file_manager.h"

GapBuffer* gap_buffer_load_from_file(const char *filename) {
    if (!filename) {
        return NULL;
    }

    FILE *file = fopen(filename, "rb");
    if (!file) return NULL;

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return NULL;
    }

    long file_size = ftell(file);
    if (file_size < 0) {
        fclose(file);
        return NULL;
    }

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

gboolean gap_buffer_save_to_file(const GapBuffer *gb, const char *filename) {
    if (!gb || !filename) {
        return FALSE;
    }

    FILE *file = fopen(filename, "wb");
    if (!file) {
        return FALSE;
    }

    if (gb->gap_start > 0) {
        if (fwrite(gb->buffer, 1, gb->gap_start, file) != gb->gap_start) {
            fclose(file);
            return FALSE;
        }
    }

    size_t suffix_len = gb->size - gb->gap_end;
    if (suffix_len > 0) {
        if (fwrite(gb->buffer + gb->gap_end, 1, suffix_len, file) != suffix_len) {
            fclose(file);
            return FALSE;
        }
    }

    fclose(file);
    return TRUE;
}