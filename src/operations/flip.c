#include <stddef.h>
#include "operations/flip.h"

void apply_horizontal_flip(Image *image)
{
    int total_rows;

    if (image == NULL || image->data == NULL)
    {
        return;
    }

    total_rows = image->height;

    for (int y = 0; y < total_rows; y++)
    {
        for (int x = 0; x < image->width / 2; x++)
        {
            Pixel *left = &image->data[y * image->width + x];
            Pixel *right =
                &image->data[y * image->width + (image->width - 1 - x)];

            Pixel temp = *left;
            *left = *right;
            *right = temp;
        }
    }
}

void apply_vertical_flip(Image *image)
{
    int total_columns;

    if (image == NULL || image->data == NULL)
    {
        return;
    }

    total_columns = image->width;

    for (int x = 0; x < total_columns; x++)
    {
        for (int y = 0; y < image->height / 2; y++)
        {
            Pixel *top = &image->data[y * image->width + x];
            Pixel *bottom =
                &image->data[(image->height - 1 - y) * image->width + x];

            Pixel temp = *top;
            *top = *bottom;
            *bottom = temp;
        }
    }
}