#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure pour stocker un état précédent du GapBuffer (pour undo)
typedef struct HistoryNode {
    char *buffer;       // Copie du buffer à ce moment
    size_t gap_start;   // Position du début du gap
    size_t gap_end;     // Position de la fin du gap
    size_t size;        // Taille totale du buffer
    struct HistoryNode *next; // Pointeur vers l'état précédent
} HistoryNode;

typedef struct {
    char *buffer;       
    size_t gap_start;   
    size_t gap_end;     
    size_t size;
    HistoryNode *history; // Pile des états précédents pour undo
    HistoryNode *redo;    // Pile des états possibles pour redo
} GapBuffer;

typedef struct {
    size_t x; // Colonne (0-based)
    size_t y; // Ligne (0-based)
} CursorPos;

GapBuffer* gap_buffer_create(size_t initial_capacity);
GapBuffer* gap_buffer_resize(GapBuffer *gb);
void gap_buffer_insert(GapBuffer *gb, char c); // Insère un caractère et sauvegarde l'état pour undo
void gap_buffer_delete(GapBuffer *gb); // Supprime un caractère et sauvegarde l'état pour undo
void gap_buffer_move_cursor(GapBuffer *gb, size_t new_position);
void gap_buffer_destroy(GapBuffer *gb); // Libère la mémoire, y compris l'historique
char* gap_buffer_get_content(GapBuffer *gb);
void gap_buffer_backspace(GapBuffer *gb);
CursorPos gap_buffer_get_cursor_pos(GapBuffer *gb);
void gap_buffer_set_cursor_pos(GapBuffer *gb, int x, int y);
void gap_buffer_push_history(GapBuffer *gb); // Sauvegarde l'état actuel dans l'historique
void gap_buffer_undo(GapBuffer *gb); // Restaure l'état précédent depuis l'historique
void gap_buffer_redo(GapBuffer *gb); // Restaure l'état suivant depuis l'historique redo
#endif 