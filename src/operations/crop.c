#include <stdlib.h>
#include "operations/crop.h"

void apply_crop(Image *image, int x, int y, int width, int height)
{
    Image *cropped;

    if (image == NULL || image->data == NULL)
    {
        return;
    }

    /* Check that the crop dimensions are valid */
    if (x < 0 || y < 0 || width <= 0 || height <= 0)
    {
        return;
    }

    /* Check that the crop stays inside the image */
    if (x + width > image->width || y + height > image->height)
    {
        return;
    }

    cropped = create_image(width, height);

    if (cropped == NULL)
    {
        return;
    }

    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            cropped->data[row * width + col] =
                image->data[(y + row) * image->width + (x + col)];
        }
    }

    free(image->data);

    image->data = cropped->data;
    image->width = cropped->width;
    image->height = cropped->height;

    free(cropped);
}