#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} Pixel;

typedef struct
{
    int width;
    int height;
    Pixel *data;
} Image;


/* Create a new image */
Image *create_image(int width, int height);

/* Free an image and its pixel data */
void destroy_image(Image *image);

/* Create a complete independent copy of an image */
Image *copy_image(const Image *source);

#endif