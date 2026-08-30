#include <stddef.h>
#include <stdlib.h>
#include "operations/rotate.h"

void apply_rotate_90(Image *image)
{
    Image *rotated;

    if (image == NULL || image->data == NULL)
    {
        return;
    }

    rotated = create_image(image->height, image->width);

    if (rotated == NULL)
    {
        return;
    }

    for (int y = 0; y < image->height; y++)
    {
        for (int x = 0; x < image->width; x++)
        {
            int new_x = image->height - 1 - y;
            int new_y = x;

            rotated->data[new_y * rotated->width + new_x] =
                image->data[y * image->width + x];
        }
    }

    free(image->data);

    image->data = rotated->data;
    image->width = rotated->width;
    image->height = rotated->height;

    free(rotated);
}