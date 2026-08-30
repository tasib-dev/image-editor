#include <stdio.h>
#include <stdlib.h>

#include <iup.h>

#include "bmp.h"
#include "image.h"

int main(void)
{
    Image *image;
    Ihandle *iup_image;
    Ihandle *label;
    Ihandle *dialog;

    unsigned char *pixels;

    image = load_bmp("test_images/lena.bmp");

    if (image == NULL)
    {
        printf("Failed to load BMP\n");
        return 1;
    }

    printf("Loaded: %d x %d\n",
           image->width,
           image->height);

    pixels = malloc(image->width * image->height * 3);

    if (pixels == NULL)
    {
        destroy_image(image);
        return 1;
    }

    for (int y = 0; y < image->height; y++)
    {
        for (int x = 0; x < image->width; x++)
        {
            Pixel *p =
                &image->data[y * image->width + x];

            int index =
                (y * image->width + x) * 3;

            pixels[index] = p->red;
            pixels[index + 1] = p->green;
            pixels[index + 2] = p->blue;
        }
    }

    IupOpen(NULL, NULL);

    iup_image = IupImageRGB(
        image->width,
        image->height,
        pixels
    );

    free(pixels);

    if (iup_image == NULL)
    {
        printf("IupImageRGB failed\n");
        destroy_image(image);
        IupClose();
        return 1;
    }

    label = IupLabel(NULL);

    IupSetAttributeHandle(
        label,
        "IMAGE",
        iup_image
    );

    IupSetAttribute(
        label,
        "RASTERSIZE",
        "512x512"
    );

    dialog = IupDialog(label);

    IupSetAttribute(
        dialog,
        "TITLE",
        "BMP Display Test"
    );

    IupSetAttribute(
        dialog,
        "MARGIN",
        "10x10"
    );

    IupShow(dialog);

    IupMainLoop();

    IupDestroy(dialog);
    IupDestroy(iup_image);

    destroy_image(image);

    IupClose();

    return 0;
}
