#ifndef UNDO_H
#define UNDO_H

#include "image.h"

/* Save the current image state */
void save_undo(const Image *image);

/* Restore the most recent saved image state */
int undo(Image *image);

/* Clear all saved undo states */
void clear_undo(void);

#endif