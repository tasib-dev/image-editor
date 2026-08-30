#include <stddef.h>
#include "operations/brightness.h"

void apply_brightness(Image *image, int amount)
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

        int red = pixel->red + amount;
        int green = pixel->green + amount;
        int blue = pixel->blue + amount;

        if (red > 255)
            red = 255;
        if (red < 0)
            red = 0;

        if (green > 255)
            green = 255;
        if (green < 0)
            green = 0;

        if (blue > 255)
            blue = 255;
        if (blue < 0)
            blue = 0;

        pixel->red = red;
        pixel->green = green;
        pixel->blue = blue;
    }
}