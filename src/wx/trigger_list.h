#ifndef TRIGGER_LIST_H
#define TRIGGER_LIST_H

#include <vector>
#include "wx/listctrl.h"
#include "../trigger.h"

class TriggerList : public wxListCtrl {
public:
  TriggerList(wxWindow *parent, wxWindowID id);

  void update();

private:
  void set_headers();
  wxString message_to_wxstring(PmMessage msg);
  wxString messages_to_wxstring(vector<PmMessage> msg);
};

#endif /* TRIGGER_LIST_H */
