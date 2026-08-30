#include <iup.h>
#include <stdio.h>

#include "gui.h"
#include "gui_crop.h"
#include "gui_image.h"
#include "operations/crop.h"


int crop_button_cb(Ihandle *ih, int button, int pressed, int x, int y, char *status)
{
    (void)ih; (void)status;

    if (button != IUP_BUTTON1 || !gui.crop_mode)
        return IUP_DEFAULT;

    if (pressed)
    {
        gui.crop_dragging = 1;
        gui.crop_have_selection = 0;
        gui.crop_start_x = x;
        gui.crop_start_y = y;
        gui.crop_end_x = x;
        gui.crop_end_y = y;
    }
    else
    {
        gui.crop_dragging = 0;
        gui.crop_end_x = x;
        gui.crop_end_y = y;
        gui.crop_have_selection = 1;
        IupRedraw(gui.image_canvas, 1);
    }

    return IUP_DEFAULT;
}

int crop_motion_cb(Ihandle *ih, int x, int y, char *status)
{
    (void)ih; (void)status;

    if (!gui.crop_mode || !gui.crop_dragging)
        return IUP_DEFAULT;

    gui.crop_end_x = x;
    gui.crop_end_y = y;

    IupRedraw(gui.image_canvas, 1);

    return IUP_DEFAULT;
}



int crop_toggle_callback(Ihandle *self)
{
    (void)self;

    gui.crop_mode = !gui.crop_mode;
    gui.crop_have_selection = 0;

    IupSetAttribute(gui.image_canvas, "CURSOR", gui.crop_mode ? "CROSS" : "ARROW");
    IupSetStrAttribute(self, "TITLE", gui.crop_mode ? "Crop: ON" : "Crop");

    IupRedraw(gui.image_canvas, 1);

    return IUP_DEFAULT;
}

int apply_crop_callback(Ihandle *self)
{
    int x0, y0, x1, y1;
    int img_x0, img_y0, img_x1, img_y1;
    int crop_w, crop_h;

    (void)self;

    if (gui.current_image == NULL)
    {
        IupMessage("Error", "Please open an image first.");
        return IUP_DEFAULT;
    }

    if (!gui.crop_mode || !gui.crop_have_selection)
    {
        IupMessage("Error", "Draw a crop selection on the image first.");
        return IUP_DEFAULT;
    }

    x0 = gui.crop_start_x < gui.crop_end_x ? gui.crop_start_x : gui.crop_end_x;
    y0 = gui.crop_start_y < gui.crop_end_y ? gui.crop_start_y : gui.crop_end_y;
    x1 = gui.crop_start_x < gui.crop_end_x ? gui.crop_end_x : gui.crop_start_x;
    y1 = gui.crop_start_y < gui.crop_end_y ? gui.crop_end_y : gui.crop_start_y;

    img_x0 = (int)((x0 - gui.display_offset_x) / gui.display_scale);
    img_y0 = (int)((y0 - gui.display_offset_y) / gui.display_scale);
    img_x1 = (int)((x1 - gui.display_offset_x) / gui.display_scale);
    img_y1 = (int)((y1 - gui.display_offset_y) / gui.display_scale);

    if (img_x0 < 0) img_x0 = 0;
    if (img_y0 < 0) img_y0 = 0;
    if (img_x1 > gui.current_image->width)  img_x1 = gui.current_image->width;
    if (img_y1 > gui.current_image->height) img_y1 = gui.current_image->height;

    crop_w = img_x1 - img_x0;
    crop_h = img_y1 - img_y0;

    if (crop_w <= 0 || crop_h <= 0)
    {
        IupMessage("Error", "Crop selection is empty or out of bounds.");
        return IUP_DEFAULT;
    }

    

    apply_crop(gui.current_image, img_x0, img_y0, crop_w, crop_h);

    gui.crop_mode = 0;
    gui.crop_have_selection = 0;
    IupSetAttribute(gui.image_canvas, "CURSOR", "ARROW");

    update_image_display(&gui);

    return IUP_DEFAULT;
}
