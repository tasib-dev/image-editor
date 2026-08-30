#ifndef GUI_CROP_H
#define GUI_CROP_H

#include <iup.h>

int crop_button_cb(Ihandle *ih, int button, int pressed, int x, int y, char *status);
int crop_motion_cb(Ihandle *ih, int x, int y, char *status);
int crop_toggle_callback(Ihandle *self);
int apply_crop_callback(Ihandle *self);

#endif