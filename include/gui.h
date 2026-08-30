#ifndef GUI_H
#define GUI_H

#include <iup.h>
#include "image.h"

typedef struct
{
    Image *current_image;
    Ihandle *current_iup_image;

    Image *brightness_base_image;

    Ihandle *image_canvas;
    Ihandle *main_dialog;

    int crop_mode;
    int crop_dragging;

    int crop_start_x;
    int crop_start_y;
    int crop_end_x;
    int crop_end_y;

    int crop_have_selection;

    double display_scale;
    int display_offset_x;
    int display_offset_y;

} GUIState;

extern GUIState gui;

int run_gui(void);

#endif