#include <stdlib.h>
#include <stdio.h>

#include <iup.h>
#include <iupdraw.h>
#include "gui.h"
#include "gui_image.h"

Ihandle *create_placeholder_image(int width, int height)
{
    unsigned char *pixels;
    Ihandle *result;
    int size;

    size = width * height * 3;

    pixels = malloc(size);

    if (pixels == NULL)
        return NULL;

    for (int i = 0; i < size; i += 3)
    {
        pixels[i]     = 200;
        pixels[i + 1] = 200;
        pixels[i + 2] = 200;
    }

    result = IupImageRGB(width, height, pixels);

    free(pixels);

    return result;
}


Ihandle *create_iup_image(const Image *image)
{
    unsigned char *pixels;
    Ihandle *result;
    int size;

    if (image == NULL || image->data == NULL)
        return NULL;

    size = image->width * image->height * 3;

    pixels = malloc(size);

    if (pixels == NULL)
        return NULL;

    for (int y = 0; y < image->height; y++)
    {
        for (int x = 0; x < image->width; x++)
        {
            Pixel *p =
                &image->data[y * image->width + x];

            int i =
                (y * image->width + x) * 3;

            pixels[i]     = p->red;
            pixels[i + 1] = p->green;
            pixels[i + 2] = p->blue;
        }
    }

    result = IupImageRGB(
        image->width,
        image->height,
        pixels
    );

    free(pixels);

    return result;
}


void update_image_display(GUIState *state)
{
    Ihandle *new_iup_image;

    if (state->current_image == NULL)
        return;

    new_iup_image =
        create_iup_image(state->current_image);

    if (new_iup_image == NULL)
    {
        IupMessage(
            "Error",
            "Failed to update image display."
        );

        return;
    }

    if (state->current_iup_image != NULL)
        IupDestroy(state->current_iup_image);

    state->current_iup_image = new_iup_image;

    IupSetHandle(
        "gui.current_image",
        state->current_iup_image
    );

    

    IupRedraw(state->image_canvas, 1);
}

int canvas_action(Ihandle *ih, float posx, float posy)
{
    int canvas_w;
    int canvas_h;
    const char *drawsize;

    int img_w;
    int img_h;

    double scale_x;
    double scale_y;

    int draw_w;
    int draw_h;

    int offset_x;
    int offset_y;

    (void)posx;
    (void)posy;

    if (gui.current_image == NULL || gui.current_iup_image == NULL)
        return IUP_DEFAULT;

    /*
     * Get the actual Canvas drawing size.
     */
    drawsize = IupGetAttribute(ih, "DRAWSIZE");

    if (drawsize == NULL ||
        sscanf(drawsize, "%dx%d", &canvas_w, &canvas_h) != 2)
    {
        return IUP_DEFAULT;
    }

    if (canvas_w <= 0 || canvas_h <= 0)
        return IUP_DEFAULT;

    img_w = gui.current_image->width;
    img_h = gui.current_image->height;

    /*
     * Calculate the scale needed to fit the image.
     */
    scale_x = (double)canvas_w / img_w;
    scale_y = (double)canvas_h / img_h;

    gui.display_scale =
        (scale_x < scale_y) ? scale_x : scale_y;

    /*
     * Don't enlarge the original image.
     */
    if (gui.display_scale > 1.0)
        gui.display_scale = 1.0;

    draw_w = (int)(img_w * gui.display_scale);
    draw_h = (int)(img_h * gui.display_scale);

    offset_x = (canvas_w - draw_w) / 2;
    offset_y = (canvas_h - draw_h) / 2;

    gui.display_offset_x = offset_x;
    gui.display_offset_y = offset_y;

    /*
     * Start drawing.
     */
    IupDrawBegin(ih);

    /*
     * Completely clear the Canvas.
     */
    IupSetAttribute(ih, "DRAWCOLOR", "0 0 0");
    IupSetAttribute(ih, "DRAWSTYLE", "FILL");

    IupDrawRectangle(
        ih,
        0,
        0,
        canvas_w - 1,
        canvas_h - 1
    );

    /*
     * Draw the current image.
     */
    IupDrawImage(
        ih,
        "gui.current_image",
        offset_x,
        offset_y,
        draw_w,
        draw_h
    );

    /*
     * Draw crop selection.
     */
    if (gui.crop_mode && (gui.crop_dragging || gui.crop_have_selection))
    {
        int x0;
        int y0;
        int x1;
        int y1;

        x0 = gui.crop_start_x < gui.crop_end_x ?
             gui.crop_start_x : gui.crop_end_x;

        y0 = gui.crop_start_y < gui.crop_end_y ?
             gui.crop_start_y : gui.crop_end_y;

        x1 = gui.crop_start_x < gui.crop_end_x ?
             gui.crop_end_x : gui.crop_start_x;

        y1 = gui.crop_start_y < gui.crop_end_y ?
             gui.crop_end_y : gui.crop_start_y;

        IupSetAttribute(ih, "DRAWCOLOR", "255 0 0");
        IupSetAttribute(ih, "DRAWSTYLE", "STROKE");

        IupDrawRectangle(
            ih,
            x0,
            y0,
            x1,
            y1
        );
    }

    IupDrawEnd(ih);

    return IUP_DEFAULT;
}