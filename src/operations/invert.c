#include <stddef.h>
#include "operations/invert.h"

void apply_invert(Image *image)
{
    int total_pixels;

    if (image == NULL || image->data == NULL)
    {
        return;
    }

    total_pixels = image->width * image->height;

    for (int i = 0; i < total_pixels; i++)
    {
        Pixel *pixel = &image->data[i];

        pixel->red = 255 - pixel->red;
        pixel->green = 255 - pixel->green;
        pixel->blue = 255 - pixel->blue;
    }
}