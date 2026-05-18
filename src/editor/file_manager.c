#include "editor/file_manager.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_MANAGER_MAX_BYTES (50U * 1024U * 1024U)

static gboolean utf8_content_valid(const char *data, size_t len) {
    if (!data) {
        return FALSE;
    }
    if (len == 0) {
        return TRUE;
    }
    return g_utf8_validate(data, (gssize)len, NULL);
}

GapBuffer *gap_buffer_load_from_file(const char *filename) {
    if (!filename) {
        return NULL;
    }

    FILE *file = fopen(filename, "rb");
    if (!file) {
        g_printerr("file_manager: cannot open '%s': %s\n", filename, strerror(errno));
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        g_printerr("file_manager: cannot seek '%s'\n", filename);
        fclose(file);
        return NULL;
    }

    long file_size_long = ftell(file);
    if (file_size_long < 0) {
        g_printerr("file_manager: cannot read size of '%s'\n", filename);
        fclose(file);
        return NULL;
    }

    if ((unsigned long)file_size_long > FILE_MANAGER_MAX_BYTES) {
        g_printerr("file_manager: '%s' exceeds %u byte limit\n",
                   filename, FILE_MANAGER_MAX_BYTES);
        fclose(file);
        return NULL;
    }

    size_t file_size = (size_t)file_size_long;

    if (fseek(file, 0, SEEK_SET) != 0) {
        g_printerr("file_manager: cannot rewind '%s'\n", filename);
        fclose(file);
        return NULL;
    }

    size_t capacity = file_size > 0 ? file_size + 64 : 64;
    GapBuffer *gb = gap_buffer_create(capacity);
    if (!gb) {
        fclose(file);
        return NULL;
    }

    if (file_size == 0) {
        fclose(file);
        gb->gap_start = 0;
        gb->gap_end = gb->size;
        return gb;
    }

    size_t read_size = fread(gb->buffer, 1, file_size, file);
    if (ferror(file)) {
        g_printerr("file_manager: read error on '%s'\n", filename);
        fclose(file);
        gap_buffer_destroy(gb);
        return NULL;
    }
    fclose(file);

    if (read_size != file_size) {
        g_printerr("file_manager: incomplete read of '%s'\n", filename);
        gap_buffer_destroy(gb);
        return NULL;
    }

    if (!utf8_content_valid(gb->buffer, read_size)) {
        g_printerr("file_manager: '%s' is not valid UTF-8\n", filename);
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
        g_printerr("file_manager: cannot write '%s': %s\n", filename, strerror(errno));
        return FALSE;
    }

    if (gb->gap_start > 0) {
        if (fwrite(gb->buffer, 1, gb->gap_start, file) != gb->gap_start) {
            g_printerr("file_manager: write error on '%s'\n", filename);
            fclose(file);
            return FALSE;
        }
    }

    size_t suffix_len = gb->size - gb->gap_end;
    if (suffix_len > 0) {
        if (fwrite(gb->buffer + gb->gap_end, 1, suffix_len, file) != suffix_len) {
            g_printerr("file_manager: write error on '%s'\n", filename);
            fclose(file);
            return FALSE;
        }
    }

    if (fflush(file) != 0 || ferror(file)) {
        g_printerr("file_manager: flush error on '%s'\n", filename);
        fclose(file);
        return FALSE;
    }

    if (fclose(file) != 0) {
        g_printerr("file_manager: close error on '%s'\n", filename);
        return FALSE;
    }

    return TRUE;
}
