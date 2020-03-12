#ifndef TRIGGER_LIST_H
#define TRIGGER_LIST_H

#include <vector>
#include "wx/listctrl.h"
#include "frame_list_ctrl.h"
#include "../trigger.h"

class TriggerList : public FrameListCtrl {
public:
  TriggerList(wxWindow *parent, wxWindowID id);

  Trigger *selected();          // may return null
  void update();

private:
  void set_headers();
};

#endif /* TRIGGER_LIST_H */
