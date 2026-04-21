#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *buffer;       
    size_t gap_start;   
    size_t gap_end;     
    size_t size; 
} GapBuffer;

typedef struct {
    size_t x; // Colonne (0-based)
    size_t y; // Ligne (0-based)
} CursorPos;

GapBuffer* gap_buffer_create(size_t initial_capacity);
GapBuffer* gap_buffer_resize(GapBuffer *gb);
void gap_buffer_insert(GapBuffer *gb, char c);
void gap_buffer_delete(GapBuffer *gb);
void gap_buffer_move_cursor(GapBuffer *gb, size_t new_position);
void gap_buffer_destroy(GapBuffer *gb);
char* gap_buffer_get_content(GapBuffer *gb);
void gap_buffer_backspace(GapBuffer *gb);
GapBuffer *gap_buffer_load_from_file(const char *filename);
CursorPos gap_buffer_get_cursor_pos(GapBuffer *gb);
void gap_buffer_set_cursor_pos(GapBuffer *gb, int x, int y);
#endif 