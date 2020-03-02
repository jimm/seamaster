#ifndef PATCH_CONNECTIONS_H
#define PATCH_CONNECTIONS_H

#include "wx/listctrl.h"
#include "../patch.h"

class PatchConnections : public wxListCtrl {
public:
  PatchConnections(wxWindow *parent, wxWindowID id);

  void update();

private:
  Patch *patch;

  void set_headers();
};

#endif /* PATCH_CONNECTIONS_H */
