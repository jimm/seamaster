#ifndef PATCH_EDITOR_H
#define PATCH_EDITOR_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif
#include "event.h"

using namespace std;

enum {
  ID_PE_Name = 6000,
  ID_PE_StartMessageDropdown,
  ID_PE_StopMessageDropdown,
  ID_PE_DoneButton
};

class Patch;
class Message;

class PatchEditor : public wxDialog {
public:
  PatchEditor(wxWindow *parent, Patch *patch);

private:
  Patch *patch;
  wxTextCtrl *name_text;
  wxComboBox *cb_start_message;
  wxComboBox *cb_stop_message;

  wxWindow *make_name_panel(wxPanel *parent);
  wxWindow *make_start_panel(wxPanel *parent);
  wxWindow *make_stop_panel(wxPanel *parent);
  wxWindow *make_message_panel(wxPanel *parent, wxWindowID id,
                               const char * const title, Message *msg,
                               wxComboBox **cb_ptr);

  void done(wxCommandEvent& event);

  wxDECLARE_EVENT_TABLE();
};

#endif /* PATCH_EDITOR_H */
