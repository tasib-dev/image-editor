#ifndef BMP_H
#define BMP_H

#include "image.h"

/* Load a 24-bit uncompressed BMP file */
Image *load_bmp(const char *filename);

/* Save an Image as a 24-bit uncompressed BMP file */
int save_bmp(const char *filename, const Image *image);

#endif