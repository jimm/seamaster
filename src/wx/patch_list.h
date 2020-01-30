#ifndef PATCH_LIST_H
#define PATCH_LIST_H

#include "wx/listctrl.h"
#include "../patch.h"

class PatchList : public wxListCtrl {
public:
  PatchList(wxWindow *parent);

  void set_patch(Patch *);

private:
  Patch *patch;

  void set_headers();
};

#endif /* PATCH_LIST_H */
