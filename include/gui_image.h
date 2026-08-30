#ifndef GUI_IMAGE_H
#define GUI_IMAGE_H

#include <iup.h>
#include "gui.h"

Ihandle *create_placeholder_image(int width, int height);
Ihandle *create_iup_image(const Image *image);
void update_image_display(GUIState *state);
int canvas_action(Ihandle *ih, float posx, float posy);

#endif