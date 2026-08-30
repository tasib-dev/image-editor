#include <stdlib.h>
#include "undo.h"

static Image *previous_image = NULL;

void save_undo(const Image *image)
{
    if (image == NULL)
    {
        return;
    }

    /* Remove the old saved state */
    if (previous_image != NULL)
    {
        destroy_image(previous_image);
        previous_image = NULL;
    }

    /* Make an independent copy */
    previous_image = copy_image(image);
}

int undo(Image *image)
{
    Image *temp;

    if (image == NULL || previous_image == NULL)
    {
        return 0;
    }

    /*
     * Save the current image data temporarily,
     * because we are going to replace it.
     */
    temp = previous_image;
    previous_image = NULL;

    free(image->data);

    image->width = temp->width;
    image->height = temp->height;
    image->data = temp->data;

    free(temp);

    return 1;
}

void clear_undo(void)
{
    if (previous_image != NULL)
    {
        destroy_image(previous_image);
        previous_image = NULL;
    }
}