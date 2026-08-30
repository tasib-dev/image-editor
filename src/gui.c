#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iup.h>
#include <iupdraw.h>

#include "gui.h"
#include "bmp.h"
#include "image.h"
#include "gui_image.h"
#include "gui_callbacks.h"
#include "gui_crop.h"
#include "operations/grayscale.h"
#include "operations/invert.h"
#include "operations/sharpen.h"
#include "operations/flip.h"
#include "operations/blur.h"
#include "operations/rotate.h"
#include "operations/crop.h"
#include "operations/brightness.h"

GUIState gui = {0};

int run_gui(void)
{
    Ihandle *open_button;
    Ihandle *save_button;
    Ihandle *grayscale_button;
    Ihandle *invert_button;
    Ihandle *brightness_button;
    Ihandle *sharpen_button;
    Ihandle *horizontal_flip_button;
    Ihandle *vertical_flip_button;
    Ihandle *blur_button;
    Ihandle *rotate_button;
    Ihandle *crop_button;
    Ihandle *apply_crop_button;
    Ihandle *layout;
    Ihandle *dialog;


    IupOpen(NULL, NULL);


    /*
     * Open button.
     */
    open_button = IupButton(
        "Open",
        NULL
    );

    IupSetCallback(
        open_button,
        "ACTION",
        (Icallback)open_callback
    );

    /*
    * Save button.
    */
    save_button = IupButton(
        "Save",
        NULL
    );

    IupSetCallback(
        save_button,
        "ACTION",
        (Icallback)save_callback
    );

    grayscale_button = IupButton(
        "Grayscale",
        NULL
    );

    IupSetCallback(
        grayscale_button,
        "ACTION",
        (Icallback)grayscale_callback
    );

    invert_button = IupButton(
        "Invert",
        NULL
    );

    IupSetCallback(
        invert_button,
        "ACTION",
        (Icallback)invert_callback
    );

    brightness_button = IupButton(
        "Brightness",
        NULL
    );

    IupSetCallback(
        brightness_button,
        "ACTION",
        (Icallback)brightness_callback
    );

    sharpen_button = IupButton(
        "Sharpen",
        NULL
    );

    IupSetCallback(
        sharpen_button,
        "ACTION",
        (Icallback)sharpen_callback
    );

    horizontal_flip_button = IupButton(
        "Flip Horizontal",
        NULL
    );

    IupSetCallback(
        horizontal_flip_button,
        "ACTION",
        (Icallback)horizontal_flip_callback
    );


    vertical_flip_button = IupButton(
        "Flip Vertical",
        NULL
    );

    IupSetCallback(
        vertical_flip_button,
        "ACTION",
        (Icallback)vertical_flip_callback
    );

    blur_button = IupButton(
        "Blur",
        NULL
    );

    IupSetCallback(
        blur_button,
        "ACTION",
        (Icallback)blur_callback
    );

    rotate_button = IupButton(
        "Rotate 90°",
        NULL
    );

    IupSetCallback(
        rotate_button,
        "ACTION",
        (Icallback)rotate_callback
    );

    crop_button = IupButton("Crop", NULL);
    IupSetCallback(crop_button, "ACTION", (Icallback)crop_toggle_callback);

    apply_crop_button = IupButton("Apply Crop", NULL);
    IupSetCallback(apply_crop_button, "ACTION", (Icallback)apply_crop_callback);

    
    gui.image_canvas = IupCanvas(NULL);

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-function-type"
        IupSetCallback(
            gui.image_canvas,
            "ACTION",
            (Icallback)canvas_action
        );
    #pragma GCC diagnostic pop

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-function-type"
        IupSetCallback(gui.image_canvas, "BUTTON_CB", (Icallback)crop_button_cb);
        IupSetCallback(gui.image_canvas, "MOTION_CB", (Icallback)crop_motion_cb);
    #pragma GCC diagnostic pop

    IupSetAttribute(gui.image_canvas, "BGCOLOR", "0 0 0");


    IupSetAttribute(
        gui.image_canvas,
        "RASTERSIZE",
        "600x450"
    );

    IupSetAttribute(
        gui.image_canvas,
        "ALIGNMENT",
        "ACENTER:ACENTER"
    );

    

    
    /*
     * Main layout.
     */
    layout = IupVbox(
        open_button,
        save_button,
        grayscale_button,
        invert_button,
        brightness_button,
        sharpen_button,
        horizontal_flip_button,
        vertical_flip_button,
        blur_button,
        rotate_button,
        crop_button,
        apply_crop_button,
        gui.image_canvas,
        NULL
    );

    IupSetAttribute(
        layout,
        "MARGIN",
        "10x10"
    );

    IupSetAttribute(
        layout,
        "GAP",
        "10"
    );

    


    /*
     * Main dialog.
     */
    dialog = IupDialog(layout);

    IupSetAttribute(
        dialog,
        "TITLE",
        "C Image Editor"
    );

    IupSetAttribute(
        dialog,
        "BGCOLOR",
        "45 45 45"
    );

    

    /*
     * Store dialog handle so open_callback can set it
     * as PARENTDIALOG on the file dialog.
     */
    gui.main_dialog = dialog;

    IupSetAttribute(
        dialog,
        "PLACEMENT",
        "NORMAL"
    );
    /*
     * Show main window.
     */
    IupShow(dialog);

    IupMainLoop();


    /*
     * Cleanup.
     */
    if (gui.current_iup_image != NULL)
        IupDestroy(gui.current_iup_image);

    if (gui.current_image != NULL)
        destroy_image(gui.current_image);

    IupDestroy(dialog);

    IupClose();

    return 0;
}