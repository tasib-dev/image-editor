#include <stdlib.h>
#include "operations/sharpen.h"

static int clamp(int value)
{
    if (value < 0)
    {
        return 0;
    }

    if (value > 255)
    {
        return 255;
    }

    return value;
}

void apply_sharpen(Image *image)
{
    Image *sharpened;

    if (image == NULL || image->data == NULL)
    {
        return;
    }

    sharpened = create_image(image->width, image->height);

    if (sharpened == NULL)
    {
        return;
    }

    for (int y = 0; y < image->height; y++)
    {
        for (int x = 0; x < image->width; x++)
        {
            int red_sum = 0;
            int green_sum = 0;
            int blue_sum = 0;

            for (int dy = -1; dy <= 1; dy++)
            {
                for (int dx = -1; dx <= 1; dx++)
                {
                    int nx = x + dx;
                    int ny = y + dy;

                    if (nx >= 0 && nx < image->width &&
                        ny >= 0 && ny < image->height)
                    {
                        int kernel;

                        if (dx == 0 && dy == 0)
                        {
                            kernel = 9;
                        }
                        else
                        {
                            kernel = -1;
                        }

                        Pixel *pixel =
                            &image->data[ny * image->width + nx];

                        red_sum += pixel->red * kernel;
                        green_sum += pixel->green * kernel;
                        blue_sum += pixel->blue * kernel;
                    }
                }
            }

            Pixel *output =
                &sharpened->data[y * sharpened->width + x];

            output->red = clamp(red_sum);
            output->green = clamp(green_sum);
            output->blue = clamp(blue_sum);
        }
    }

    free(image->data);

    image->data = sharpened->data;

    free(sharpened);
}