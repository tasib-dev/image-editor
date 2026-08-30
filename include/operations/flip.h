#ifndef FLIP_H
#define FLIP_H

#include "image.h"

/* Flip an image from left to right */
void apply_horizontal_flip(Image *image);

/* Flip an image from top to bottom */
void apply_vertical_flip(Image *image);

#endif