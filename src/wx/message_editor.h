#ifndef MESSAGE_EDITOR_H
#define MESSAGE_EDITOR_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif
#include "events.h"

enum {
  ID_ME_Name = 2000,
  ID_ME_MessageText,
  ID_ME_DoneButton
};

class PatchMaster;
class Message;
class wxListCtrl;

class MessageEditor : public wxDialog {
public:
  MessageEditor(wxWindow *parent, Message *message);

private:
  PatchMaster *pm;
  Message *message;
  wxTextCtrl *name_text;
  wxTextCtrl *message_text;

  wxWindow *make_name_panel(wxPanel *parent);

  wxString messages_to_text();

  void done(wxCommandEvent& event);

  wxDECLARE_EVENT_TABLE();
};

#endif /* MESSAGE_EDITOR_H */
