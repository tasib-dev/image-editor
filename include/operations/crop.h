#ifndef CROP_H
#define CROP_H

#include "image.h"

/* Crop an image to a rectangular region */

void apply_crop(Image *image, int x, int y, int width, int height);

#endif