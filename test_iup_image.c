#include <stdlib.h>
#include <iup.h>

int main(void)
{
    Ihandle *image;
    Ihandle *label;
    Ihandle *dialog;

    unsigned char *pixels;
    int width = 512;
    int height = 512;

    pixels = malloc(width * height * 3);

    if (pixels == NULL)
    {
        return 1;
    }

    /*
     * Create a simple red image.
     */
    for (int i = 0; i < width * height; i++)
    {
        pixels[i * 3] = 255;
        pixels[i * 3 + 1] = 0;
        pixels[i * 3 + 2] = 0;
    }

    IupOpen(NULL, NULL);

    image = IupImageRGB(width, height, pixels);

    free(pixels);

    if (image == NULL)
    {
        IupMessage("Error", "IupImageRGB failed.");
        IupClose();
        return 1;
    }

    label = IupLabel(NULL);

    IupSetAttributeHandle(
        label,
        "IMAGE",
        image
    );

    dialog = IupDialog(label);

    IupSetAttribute(
        dialog,
        "TITLE",
        "IUP Image Test"
    );

    IupSetAttribute(
        dialog,
        "SIZE",
        "600x600"
    );

    IupShow(dialog);

    IupMainLoop();

    IupDestroy(dialog);
    IupDestroy(image);

    IupClose();

    return 0;
}
