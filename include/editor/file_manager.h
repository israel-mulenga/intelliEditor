#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <glib.h>
#include "editor/gap_buffer.h"

GapBuffer *gap_buffer_load_from_file(const char *filename);
gboolean gap_buffer_save_to_file(const GapBuffer *gb, const char *filename);

#endif
