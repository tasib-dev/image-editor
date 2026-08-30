#include <iup.h>
#include <stdio.h>
#include <string.h>

#include "gui.h"
#include "gui_image.h"
#include "bmp.h"
#include "operations/grayscale.h"
#include "operations/invert.h"
#include "operations/sharpen.h"
#include "operations/flip.h"
#include "operations/blur.h"
#include "operations/rotate.h"
#include "operations/brightness.h"
#include "image.h"

static void clear_brightness_base(void);

int open_callback(Ihandle *self)
{
    Ihandle *file_dialog;
    const char *filename;

    Image *new_image;
    Ihandle *new_iup_image;

    (void)self;


    /* Open file dialog */
    file_dialog = IupFileDlg();

    IupSetAttribute(
        file_dialog,
        "DIALOGTYPE",
        "OPEN"
    );

    IupSetAttribute(
        file_dialog,
        "EXTFILTER",
        "BMP files|*.bmp|All files|*.*"
    );

    IupSetAttribute(
        file_dialog,
        "TITLE",
        "Open BMP Image"
    );

    /*
     * IMPORTANT:
     * Give the file dialog a parent dialog so it behaves
     * correctly as a modal child window on all platforms.
     */
    IupSetAttributeHandle(
        file_dialog,
        "PARENTDIALOG",
        gui.main_dialog
    );

    IupPopup(
        file_dialog,
        IUP_CENTER,
        IUP_CENTER
    );

    

    /* User cancelled */
    if (IupGetInt(file_dialog, "STATUS") == -1)
    {
        IupDestroy(file_dialog);
        return IUP_DEFAULT;
    }


    filename =
        IupGetAttribute(
            file_dialog,
            "VALUE"
        );

    

    if (filename == NULL)
    {
        IupDestroy(file_dialog);
        return IUP_DEFAULT;
    }


    /* Load BMP */
    new_image = load_bmp(filename);

    

    if (new_image == NULL)
    {
        IupDestroy(file_dialog);

        IupMessage(
            "Error",
            "Failed to load BMP image."
        );

        return IUP_DEFAULT;
    }


    IupDestroy(file_dialog);

    

    /* Convert Image -> IUP Image */
    new_iup_image =
        create_iup_image(new_image);

    if (new_iup_image == NULL)
    {
        destroy_image(new_image);

        IupMessage(
            "Error",
            "Failed to create image display."
        );

        return IUP_DEFAULT;
    }


    /*
     * Delete previous image.
     */
    if (gui.current_image != NULL)
        destroy_image(gui.current_image);

    if (gui.current_iup_image != NULL)
        IupDestroy(gui.current_iup_image);


    /*
     * Store new image.
     */
    gui.current_image = new_image;
    gui.current_iup_image = new_iup_image;

    IupSetHandle(
        "gui.current_image",
        gui.current_iup_image
    );


    /*
     * IMPORTANT:
     * gui.image_canvas must ALWAYS be in image mode.
     * Never touch TITLE - just swap IMAGE.
     */
    IupSetAttributeHandle(
        gui.image_canvas,
        "IMAGE",
        gui.current_iup_image
    );

    

    /*
     * Force IUP to recalculate and redraw.
     */
    IupRedraw(gui.image_canvas, 1);


    return IUP_DEFAULT;
}

int save_callback(Ihandle *self)
{
    Ihandle *file_dialog;
    const char *filename;

    (void)self;

    if (gui.current_image == NULL)
    {
        IupMessage(
            "Error",
            "Please open an image first."
        );

        return IUP_DEFAULT;
    }

    /* Create save file dialog */
    file_dialog = IupFileDlg();

    IupSetAttribute(
        file_dialog,
        "DIALOGTYPE",
        "SAVE"
    );

    IupSetAttribute(
        file_dialog,
        "EXTFILTER",
        "BMP files|*.bmp|All files|*.*"
    );

    IupSetAttribute(
        file_dialog,
        "TITLE",
        "Save BMP Image"
    );

    /*
     * Make the file dialog a child of the main window.
     */
    IupSetAttributeHandle(
        file_dialog,
        "PARENTDIALOG",
        gui.main_dialog
    );

    /*
     * Default filename.
     */
    IupSetAttribute(
        file_dialog,
        "VALUE",
        "edited_image.bmp"
    );

    IupPopup(
        file_dialog,
        IUP_CENTER,
        IUP_CENTER
    );

    /* User cancelled */
    if (IupGetInt(file_dialog, "STATUS") == -1)
    {
        IupDestroy(file_dialog);
        return IUP_DEFAULT;
    }

    filename = IupGetAttribute(
        file_dialog,
        "VALUE"
    );

    if (filename == NULL || filename[0] == '\0')
    {
        IupDestroy(file_dialog);
        return IUP_DEFAULT;
    }

    /*
     * Save the current edited image.
     */
    if (!save_bmp(filename, gui.current_image))
    {
        IupDestroy(file_dialog);

        IupMessage(
            "Error",
            "Failed to save BMP image."
        );

        return IUP_DEFAULT;
    }

    IupDestroy(file_dialog);

    IupMessage(
        "Success",
        "Image saved successfully."
    );

    return IUP_DEFAULT;
}

int grayscale_callback(Ihandle *self)
{
    (void)self;

    if (gui.current_image == NULL)
    {
        IupMessage(
            "Error",
            "Please open an image first."
        );

        return IUP_DEFAULT;
    }

    clear_brightness_base();

    apply_grayscale(gui.current_image);

    update_image_display(&gui);

    return IUP_DEFAULT;
}

static void clear_brightness_base(void)
{
    if (gui.brightness_base_image != NULL)
    {
        destroy_image(gui.brightness_base_image);
        gui.brightness_base_image = NULL;
    }
}

int invert_callback(Ihandle *self)
{
    (void)self;

    if (gui.current_image == NULL)
    {
        IupMessage(
            "Error",
            "Please open an image first."
        );

        return IUP_DEFAULT;
    }

    

    apply_invert(gui.current_image);

    update_image_display(&gui);

    return IUP_DEFAULT;
}

int sharpen_callback(Ihandle *self)
{
    (void)self;

    if (gui.current_image == NULL)
    {
        IupMessage(
            "Error",
            "Please open an image first."
        );

        return IUP_DEFAULT;
    }

    

    apply_sharpen(gui.current_image);

    update_image_display(&gui);

    return IUP_DEFAULT;
}

int horizontal_flip_callback(Ihandle *self)
{
    (void)self;

    if (gui.current_image == NULL)
    {
        IupMessage(
            "Error",
            "Please open an image first."
        );

        return IUP_DEFAULT;
    }

    

    apply_horizontal_flip(gui.current_image);

    update_image_display(&gui);

    return IUP_DEFAULT;
}


int vertical_flip_callback(Ihandle *self)
{
    (void)self;

    if (gui.current_image == NULL)
    {
        IupMessage(
            "Error",
            "Please open an image first."
        );

        return IUP_DEFAULT;
    }

    

    apply_vertical_flip(gui.current_image);

    update_image_display(&gui);

    return IUP_DEFAULT;
}

int blur_callback(Ihandle *self)
{
    (void)self;

    if (gui.current_image == NULL)
    {
        IupMessage(
            "Error",
            "Please open an image first."
        );

        return IUP_DEFAULT;
    }


    apply_blur(gui.current_image);

    update_image_display(&gui);

    return IUP_DEFAULT;
}

int rotate_callback(Ihandle *self)
{
    (void)self;

    if (gui.current_image == NULL)
    {
        IupMessage(
            "Error",
            "Please open an image first."
        );

        return IUP_DEFAULT;
    }
    

    apply_rotate_90(gui.current_image);

    update_image_display(&gui);

    return IUP_DEFAULT;
}

int brightness_callback(Ihandle *self)
{
    int amount = 30;

    (void)self;

    if (gui.current_image == NULL)
    {
        IupMessage(
            "Error",
            "Please open an image first."
        );

        return IUP_DEFAULT;
    }

    /*
     * Save the image before the first brightness adjustment.
     * Repeated brightness changes will always use this image
     * as the starting point.
     */
    if (gui.brightness_base_image == NULL)
    {
        gui.brightness_base_image = copy_image(gui.current_image);

        if (gui.brightness_base_image == NULL)
        {
            IupMessage(
                "Error",
                "Failed to create brightness backup."
            );

            return IUP_DEFAULT;
        }
    }

    if (!IupGetParam(
            "Brightness",
            NULL,
            NULL,
            "Amount: %i[-255,255]\n",
            &amount))
    {
        return IUP_DEFAULT;
    }

    /*
     * Restore the original state before applying
     * the newly selected brightness amount.
     */
    memcpy(
        gui.current_image->data,
        gui.brightness_base_image->data,
        gui.current_image->width *
        gui.current_image->height *
        sizeof(Pixel)
    );

    apply_brightness(gui.current_image, amount);

    update_image_display(&gui);

    return IUP_DEFAULT;
}