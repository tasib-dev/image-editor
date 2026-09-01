#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iup.h>
#include <iupdraw.h>

#include "gui.h"
#include "bmp.h"
#include "image.h"
#include "undo.h"
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
    Ihandle *undo_button;
    Ihandle *layout;
    Ihandle *dialog;
    Ihandle *button_panel;
    Ihandle *file_group;
    Ihandle *edit_group;
    Ihandle *transform_group;
    Ihandle *crop_group;


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

    undo_button = IupButton(
        "Undo",
        NULL
    );

    IupSetCallback(
        undo_button,
        "ACTION",
        (Icallback)undo_callback
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
        "700x500"
    );

    IupSetAttribute(
        gui.image_canvas,
        "ALIGNMENT",
        "ACENTER:ACENTER"
    );

    IupSetAttribute(open_button, "RASTERSIZE", "180x35");
    IupSetAttribute(save_button, "RASTERSIZE", "180x35");
    IupSetAttribute(undo_button, "RASTERSIZE", "180x35");

    IupSetAttribute(grayscale_button, "RASTERSIZE", "180x35");
    IupSetAttribute(invert_button, "RASTERSIZE", "180x35");
    IupSetAttribute(brightness_button, "RASTERSIZE", "180x35");
    IupSetAttribute(sharpen_button, "RASTERSIZE", "180x35");

    IupSetAttribute(horizontal_flip_button, "RASTERSIZE", "180x35");
    IupSetAttribute(vertical_flip_button, "RASTERSIZE", "180x35");
    IupSetAttribute(blur_button, "RASTERSIZE", "180x35");
    IupSetAttribute(rotate_button, "RASTERSIZE", "180x35");

    IupSetAttribute(crop_button, "RASTERSIZE", "180x35");
    IupSetAttribute(apply_crop_button, "RASTERSIZE", "180x35");

    file_group = IupFrame(
        IupVbox(
            open_button,
            save_button,
            NULL
        )
    );

    IupSetAttribute(file_group, "TITLE", "File");
    IupSetAttribute(file_group, "MARGIN", "10x10");

    /*
    * Edit block.
    */
    edit_group = IupFrame(
        IupVbox(
            undo_button,
            grayscale_button,
            invert_button,
            brightness_button,
            sharpen_button,
            blur_button,
            NULL
        )
    );

    IupSetAttribute(edit_group, "TITLE", "Edit");
    IupSetAttribute(edit_group, "MARGIN", "10x10");

    /*
    * Transform block.
    */
    transform_group = IupFrame(
        IupVbox(
            horizontal_flip_button,
            vertical_flip_button,
            rotate_button,
            NULL
        )
    );

    IupSetAttribute(transform_group, "TITLE", "Transform");
    IupSetAttribute(transform_group, "MARGIN", "10x10");

    /*
    * Crop block.
    */
    crop_group = IupFrame(
        IupVbox(
            crop_button,
            apply_crop_button,
            NULL
        )
    );

    IupSetAttribute(crop_group, "TITLE", "Crop");
    IupSetAttribute(crop_group, "MARGIN", "10x10");


    /*
    * Left panel containing all buttons.
    */
    button_panel = IupVbox(
        file_group,
        edit_group,
        transform_group,
        crop_group,
        NULL
    );

    IupSetAttribute(
        button_panel,
        "MARGIN",
        "10x10"
    );

    IupSetAttribute(
        button_panel,
        "GAP",
        "8"
    );


    /*
    * Main layout:
    * buttons on the left, image on the right.
    */
    layout = IupHbox(
        button_panel,
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
        "20"
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

    clear_undo();    

    IupDestroy(dialog);

    IupClose();

    return 0;
}