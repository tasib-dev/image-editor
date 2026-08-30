#include <stdlib.h>
#include "operations/blur.h"

void apply_blur(Image *image)
{
    Image *blurred;

    if (image == NULL || image->data == NULL)
    {
        return;
    }

    blurred = create_image(image->width, image->height);

    if (blurred == NULL)
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
            int count = 0;

            for (int dy = -1; dy <= 1; dy++)
            {
                for (int dx = -1; dx <= 1; dx++)
                {
                    int nx = x + dx;
                    int ny = y + dy;

                    if (nx >= 0 && nx < image->width &&
                        ny >= 0 && ny < image->height)
                    {
                        Pixel *pixel =
                            &image->data[ny * image->width + nx];

                        red_sum += pixel->red;
                        green_sum += pixel->green;
                        blue_sum += pixel->blue;

                        count++;
                    }
                }
            }

            Pixel *output =
                &blurred->data[y * blurred->width + x];

            output->red = red_sum / count;
            output->green = green_sum / count;
            output->blue = blue_sum / count;
        }
    }

    free(image->data);

    image->data = blurred->data;

    free(blurred);
}