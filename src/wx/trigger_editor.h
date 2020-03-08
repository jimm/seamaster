#ifndef TRIGGER_EDITOR_H
#define TRIGGER_EDITOR_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif
#include "events.h"

enum {
  ID_TE_InputDropdown = 3000,
  ID_TE_MessageText,
  ID_TE_ActionDropdown,
  ID_TE_DoneButton
};

class PatchMaster;
class Trigger;
class Input;
class wxListCtrl;

class TriggerEditor : public wxDialog {
public:
  TriggerEditor(wxWindow *parent, Trigger *trigger);

private:
  PatchMaster *pm;
  Trigger *trigger;
  Input *orig_input;
  wxComboBox *lc_input;
  wxTextCtrl *tc_trigger_message;
  wxComboBox *lc_action;

  wxWindow *make_input_dropdown(wxPanel *parent);
  wxWindow *make_action_dropdown(wxPanel *parent);

  void done(wxCommandEvent& event);

  wxDECLARE_EVENT_TABLE();
};

#endif /* TRIGGER_EDITOR_H */
