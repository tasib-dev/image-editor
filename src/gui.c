#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iup.h>
#include <iupdraw.h>

#include "gui.h"
#include "bmp.h"
#include "image.h"
#include "operations/grayscale.h"
#include "operations/invert.h"
#include "operations/sharpen.h"
#include "operations/flip.h"
#include "operations/blur.h"
#include "operations/rotate.h"
#include "operations/crop.h"
#include "operations/brightness.h"


static Image *current_image = NULL;
static Ihandle *current_iup_image = NULL;
static Image *brightness_base_image = NULL;
static void clear_brightness_base(void);

static Ihandle *image_canvas = NULL;
static Ihandle *main_dialog = NULL;
static int crop_mode = 0;
static int crop_dragging = 0;

static int crop_start_x = 0;
static int crop_start_y = 0;
static int crop_end_x = 0;
static int crop_end_y = 0;
static int crop_have_selection = 0;


/*
 * Create a small solid-color placeholder image so the
 * Label widget is created in "image mode" from the start.
 *
 * IMPORTANT:
 * IUP (on GTK especially) cannot switch a Label between
 * text mode and image mode after it has been mapped/shown.
 * So we must never give image_canvas a TITLE - it must be
 * an image-label from creation, and we only ever swap the
 * IMAGE handle afterward.
 */
static Ihandle *create_placeholder_image(int width, int height)
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
        pixels[i]     = 200; /* light gray */
        pixels[i + 1] = 200;
        pixels[i + 2] = 200;
    }

    result = IupImageRGB(width, height, pixels);

    free(pixels);

    return result;
}


/*
 * Convert our Image to an IUP RGB image.
 */
static Ihandle *create_iup_image(const Image *image)
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

static void update_image_display(void)
{
    Ihandle *new_iup_image;

    if (current_image == NULL)
        return;

    new_iup_image = create_iup_image(current_image);

    if (new_iup_image == NULL)
    {
        IupMessage(
            "Error",
            "Failed to update image display."
        );

        return;
    }

    if (current_iup_image != NULL)
        IupDestroy(current_iup_image);

    current_iup_image = new_iup_image;

    IupSetHandle(
        "current_image",
        current_iup_image
    );

    IupRedraw(image_canvas, 1);
}


/*
 * Open button callback.
 */
static int open_callback(Ihandle *self)
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
        main_dialog
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

    clear_brightness_base();

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
    if (current_image != NULL)
        destroy_image(current_image);

    if (current_iup_image != NULL)
        IupDestroy(current_iup_image);


    /*
     * Store new image.
     */
    current_image = new_image;
    current_iup_image = new_iup_image;

    IupSetHandle(
        "current_image",
        current_iup_image
    );


    /*
     * IMPORTANT:
     * image_canvas must ALWAYS be in image mode.
     * Never touch TITLE - just swap IMAGE.
     */
    IupSetAttributeHandle(
        image_canvas,
        "IMAGE",
        current_iup_image
    );

    

    /*
     * Force IUP to recalculate and redraw.
     */
    IupRedraw(image_canvas, 1);


    return IUP_DEFAULT;
}

/*
 * Save button callback.
 */
static int save_callback(Ihandle *self)
{
    Ihandle *file_dialog;
    const char *filename;

    (void)self;

    if (current_image == NULL)
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
        main_dialog
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
    if (!save_bmp(filename, current_image))
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



static int grayscale_callback(Ihandle *self)
{
    (void)self;

    if (current_image == NULL)
    {
        IupMessage(
            "Error",
            "Please open an image first."
        );

        return IUP_DEFAULT;
    }

    clear_brightness_base();

    apply_grayscale(current_image);

    update_image_display();

    return IUP_DEFAULT;
}


static int invert_callback(Ihandle *self)
{
    (void)self;

    if (current_image == NULL)
    {
        IupMessage(
            "Error",
            "Please open an image first."
        );

        return IUP_DEFAULT;
    }

    clear_brightness_base();

    apply_invert(current_image);

    update_image_display();

    return IUP_DEFAULT;
}



static void clear_brightness_base(void)
{
    if (brightness_base_image != NULL)
    {
        destroy_image(brightness_base_image);
        brightness_base_image = NULL;
    }
}


static int brightness_callback(Ihandle *self)
{
    int amount = 30;

    (void)self;

    if (current_image == NULL)
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
    if (brightness_base_image == NULL)
    {
        brightness_base_image = copy_image(current_image);

        if (brightness_base_image == NULL)
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
        current_image->data,
        brightness_base_image->data,
        current_image->width *
        current_image->height *
        sizeof(Pixel)
    );

    apply_brightness(current_image, amount);

    update_image_display();

    return IUP_DEFAULT;
}

static int sharpen_callback(Ihandle *self)
{
    (void)self;

    if (current_image == NULL)
    {
        IupMessage(
            "Error",
            "Please open an image first."
        );

        return IUP_DEFAULT;
    }

    clear_brightness_base();

    apply_sharpen(current_image);

    update_image_display();

    return IUP_DEFAULT;
}

static int horizontal_flip_callback(Ihandle *self)
{
    (void)self;

    if (current_image == NULL)
    {
        IupMessage(
            "Error",
            "Please open an image first."
        );

        return IUP_DEFAULT;
    }

    clear_brightness_base();

    apply_horizontal_flip(current_image);

    update_image_display();

    return IUP_DEFAULT;
}


static int vertical_flip_callback(Ihandle *self)
{
    (void)self;

    if (current_image == NULL)
    {
        IupMessage(
            "Error",
            "Please open an image first."
        );

        return IUP_DEFAULT;
    }

    clear_brightness_base();

    apply_vertical_flip(current_image);

    update_image_display();

    return IUP_DEFAULT;
}

static int blur_callback(Ihandle *self)
{
    (void)self;

    if (current_image == NULL)
    {
        IupMessage(
            "Error",
            "Please open an image first."
        );

        return IUP_DEFAULT;
    }

    clear_brightness_base();

    apply_blur(current_image);

    update_image_display();

    return IUP_DEFAULT;
}

static int rotate_callback(Ihandle *self)
{
    (void)self;

    if (current_image == NULL)
    {
        IupMessage(
            "Error",
            "Please open an image first."
        );

        return IUP_DEFAULT;
    }
    
    clear_brightness_base();
    apply_rotate_90(current_image);

    update_image_display();

    return IUP_DEFAULT;
}

static double display_scale = 1.0;
static int display_offset_x = 0;
static int display_offset_y = 0;

static int canvas_action(Ihandle *ih, float posx, float posy)
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

    if (current_image == NULL || current_iup_image == NULL)
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

    img_w = current_image->width;
    img_h = current_image->height;

    /*
     * Calculate the scale needed to fit the image.
     */
    scale_x = (double)canvas_w / img_w;
    scale_y = (double)canvas_h / img_h;

    display_scale =
        (scale_x < scale_y) ? scale_x : scale_y;

    /*
     * Don't enlarge the original image.
     */
    if (display_scale > 1.0)
        display_scale = 1.0;

    draw_w = (int)(img_w * display_scale);
    draw_h = (int)(img_h * display_scale);

    offset_x = (canvas_w - draw_w) / 2;
    offset_y = (canvas_h - draw_h) / 2;

    display_offset_x = offset_x;
    display_offset_y = offset_y;

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
        "current_image",
        offset_x,
        offset_y,
        draw_w,
        draw_h
    );

    /*
     * Draw crop selection.
     */
    if (crop_mode && (crop_dragging || crop_have_selection))
    {
        int x0;
        int y0;
        int x1;
        int y1;

        x0 = crop_start_x < crop_end_x ?
             crop_start_x : crop_end_x;

        y0 = crop_start_y < crop_end_y ?
             crop_start_y : crop_end_y;

        x1 = crop_start_x < crop_end_x ?
             crop_end_x : crop_start_x;

        y1 = crop_start_y < crop_end_y ?
             crop_end_y : crop_start_y;

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


static int crop_button_cb(Ihandle *ih, int button, int pressed, int x, int y, char *status)
{
    (void)ih; (void)status;

    if (button != IUP_BUTTON1 || !crop_mode)
        return IUP_DEFAULT;

    if (pressed)
    {
        crop_dragging = 1;
        crop_have_selection = 0;
        crop_start_x = x;
        crop_start_y = y;
        crop_end_x = x;
        crop_end_y = y;
    }
    else
    {
        crop_dragging = 0;
        crop_end_x = x;
        crop_end_y = y;
        crop_have_selection = 1;
        IupRedraw(image_canvas, 1);
    }

    return IUP_DEFAULT;
}

static int crop_motion_cb(Ihandle *ih, int x, int y, char *status)
{
    (void)ih; (void)status;

    if (!crop_mode || !crop_dragging)
        return IUP_DEFAULT;

    crop_end_x = x;
    crop_end_y = y;

    IupRedraw(image_canvas, 1);

    return IUP_DEFAULT;
}



static int crop_toggle_callback(Ihandle *self)
{
    (void)self;

    crop_mode = !crop_mode;
    crop_have_selection = 0;

    IupSetAttribute(image_canvas, "CURSOR", crop_mode ? "CROSS" : "ARROW");
    IupSetStrAttribute(self, "TITLE", crop_mode ? "Crop: ON" : "Crop");

    IupRedraw(image_canvas, 1);

    return IUP_DEFAULT;
}

static int apply_crop_callback(Ihandle *self)
{
    int x0, y0, x1, y1;
    int img_x0, img_y0, img_x1, img_y1;
    int crop_w, crop_h;

    (void)self;

    if (current_image == NULL)
    {
        IupMessage("Error", "Please open an image first.");
        return IUP_DEFAULT;
    }

    if (!crop_mode || !crop_have_selection)
    {
        IupMessage("Error", "Draw a crop selection on the image first.");
        return IUP_DEFAULT;
    }

    x0 = crop_start_x < crop_end_x ? crop_start_x : crop_end_x;
    y0 = crop_start_y < crop_end_y ? crop_start_y : crop_end_y;
    x1 = crop_start_x < crop_end_x ? crop_end_x : crop_start_x;
    y1 = crop_start_y < crop_end_y ? crop_end_y : crop_start_y;

    img_x0 = (int)((x0 - display_offset_x) / display_scale);
    img_y0 = (int)((y0 - display_offset_y) / display_scale);
    img_x1 = (int)((x1 - display_offset_x) / display_scale);
    img_y1 = (int)((y1 - display_offset_y) / display_scale);

    if (img_x0 < 0) img_x0 = 0;
    if (img_y0 < 0) img_y0 = 0;
    if (img_x1 > current_image->width)  img_x1 = current_image->width;
    if (img_y1 > current_image->height) img_y1 = current_image->height;

    crop_w = img_x1 - img_x0;
    crop_h = img_y1 - img_y0;

    if (crop_w <= 0 || crop_h <= 0)
    {
        IupMessage("Error", "Crop selection is empty or out of bounds.");
        return IUP_DEFAULT;
    }

    clear_brightness_base();

    apply_crop(current_image, img_x0, img_y0, crop_w, crop_h);

    crop_mode = 0;
    crop_have_selection = 0;
    IupSetAttribute(image_canvas, "CURSOR", "ARROW");

    update_image_display();

    return IUP_DEFAULT;
}


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

    

    /*
     * Permanent image display area.
     *
     * IMPORTANT:
     * It exists before the dialog is shown, and it is
     * created in IMAGE mode from the start (never given
     * a TITLE), because IUP cannot switch a Label from
     * text mode to image mode after it has been mapped.
     */
    image_canvas = IupCanvas(NULL);

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-function-type"
        IupSetCallback(
            image_canvas,
            "ACTION",
            (Icallback)canvas_action
        );
    #pragma GCC diagnostic pop

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-function-type"
        IupSetCallback(image_canvas, "BUTTON_CB", (Icallback)crop_button_cb);
        IupSetCallback(image_canvas, "MOTION_CB", (Icallback)crop_motion_cb);
    #pragma GCC diagnostic pop

    IupSetAttribute(image_canvas, "BGCOLOR", "0 0 0");


    IupSetAttribute(
        image_canvas,
        "RASTERSIZE",
        "600x450"
    );

    IupSetAttribute(
        image_canvas,
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
        image_canvas,
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
    main_dialog = dialog;

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
    if (current_iup_image != NULL)
        IupDestroy(current_iup_image);

    if (current_image != NULL)
        destroy_image(current_image);

    IupDestroy(dialog);

    IupClose();

    return 0;
}