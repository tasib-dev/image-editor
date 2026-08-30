#ifndef UNDO_H
#define UNDO_H

#include "image.h"

/* Save the current image state */
void save_undo(const Image *image);

/* Restore the previously saved image state */
int undo(Image *image);

/* Clear the saved undo state */
void clear_undo(void);

#endif