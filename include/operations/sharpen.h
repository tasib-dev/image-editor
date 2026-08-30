#ifndef SHARPEN_H
#define SHARPEN_H

#include "image.h"

/* Sharpen an image using a 3x3 convolution kernel */

void apply_sharpen(Image *image);

#endif