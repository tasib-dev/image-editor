#include <stdlib.h>
#include "undo.h"

#define UNDO_LEVELS 3

static Image *undo_history[UNDO_LEVELS] = {NULL, NULL, NULL};

void save_undo(const Image *image)
{
    if (image == NULL)
    {
        return;
    }

    /*
     * Remove the oldest undo state.
     */
    if (undo_history[UNDO_LEVELS - 1] != NULL)
    {
        destroy_image(undo_history[UNDO_LEVELS - 1]);
        undo_history[UNDO_LEVELS - 1] = NULL;
    }

    /*
     * Shift existing states toward the oldest position.
     *
     * history[0] = newest
     * history[1] = second newest
     * history[2] = oldest
     */
    for (int i = UNDO_LEVELS - 1; i > 0; i--)
    {
        undo_history[i] = undo_history[i - 1];
    }

    /*
     * Save an independent copy as the newest state.
     */
    undo_history[0] = copy_image(image);
}

int undo(Image *image)
{
    Image *temp;

    if (image == NULL || undo_history[0] == NULL)
    {
        return 0;
    }

    /*
     * Take the newest saved state.
     */
    temp = undo_history[0];

    /*
     * Shift the remaining states forward.
     */
    for (int i = 0; i < UNDO_LEVELS - 1; i++)
    {
        undo_history[i] = undo_history[i + 1];
    }

    undo_history[UNDO_LEVELS - 1] = NULL;

    /*
     * Replace the current image with the saved image.
     */
    free(image->data);

    image->width = temp->width;
    image->height = temp->height;
    image->data = temp->data;

    free(temp);

    return 1;
}

void clear_undo(void)
{
    for (int i = 0; i < UNDO_LEVELS; i++)
    {
        if (undo_history[i] != NULL)
        {
            destroy_image(undo_history[i]);
            undo_history[i] = NULL;
        }
    }
}