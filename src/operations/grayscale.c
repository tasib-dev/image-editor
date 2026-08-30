#include <stddef.h>
#include "operations/grayscale.h"

void apply_grayscale(Image *image)
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

        uint8_t gray =
            0.299 * pixel->red +
            0.587 * pixel->green +
            0.114 * pixel->blue;

        pixel->red = gray;
        pixel->green = gray;
        pixel->blue = gray;
    }
}