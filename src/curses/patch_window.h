#ifndef PATCH_WINDOW_H
#define PATCH_WINDOW_H

#include "window.h"
#include "../patch.h"

class PatchWindow : public Window {
public:
  Patch *patch;

  PatchWindow(struct rect, const char *);
  ~PatchWindow();

  void set_contents(Patch *patch);

  void draw();

private:
  void draw_headers();
  void draw_connection(Connection *);
  void format_chans(Connection *, char *);
  void format_zone(Connection *, char *);
  void format_xpose(Connection *, char *);
  void format_prog(Connection *, char *);
};

#endif /* PATCH_WINDOW_H */
