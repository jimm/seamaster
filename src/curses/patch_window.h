#ifndef PATCH_WINDOW_H
#define PATCH_WINDOW_H

#include "window.h"
#include "../patch.h"

typedef struct patch_window {
  window *w;
  patch *patch;
} patch_window;

patch_window *patch_window_new(rect, char *);
void patch_window_free(patch_window *);

void patch_window_set_contents(patch_window *, patch *patch);

void patch_window_draw(patch_window *);

#endif /* PATCH_WINDOW_H */
