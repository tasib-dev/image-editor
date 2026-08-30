#include <stdio.h>
#include <stdlib.h>
#include "image.h"


Image *create_image(int width, int height)
{
    Image *image;

    if (width <= 0 || height <= 0)
    {
        return NULL;
    }

    image = malloc(sizeof(Image));

    if (image == NULL)
    {
        return NULL;
    }

    image->width = width;
    image->height = height;

    image->data = malloc(width * height * sizeof(Pixel));

    if (image->data == NULL)
    {
        free(image);
        return NULL;
    }

    return image;
}


void destroy_image(Image *image)
{
    if (image == NULL)
    {
        return;
    }

    free(image->data);
    free(image);
}


Image *copy_image(const Image *source)
{
    Image *copy;
    int total_pixels;

    if (source == NULL)
    {
        return NULL;
    }

    copy = create_image(source->width, source->height);

    if (copy == NULL)
    {
        return NULL;
    }

    total_pixels = source->width * source->height;

    for (int i = 0; i < total_pixels; i++)
    {
        copy->data[i] = source->data[i];
    }

    return copy;
}