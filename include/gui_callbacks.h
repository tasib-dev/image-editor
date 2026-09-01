#ifndef GUI_CALLBACKS_H
#define GUI_CALLBACKS_H

#include <iup.h>

int open_callback(Ihandle *self);
int save_callback(Ihandle *self);
int grayscale_callback(Ihandle *self);
int invert_callback(Ihandle *self);
int sharpen_callback(Ihandle *self);
int horizontal_flip_callback(Ihandle *self);
int vertical_flip_callback(Ihandle *self);
int blur_callback(Ihandle *self);
int rotate_callback(Ihandle *self);
int brightness_callback(Ihandle *self);
int undo_callback(Ihandle *self);

#endif