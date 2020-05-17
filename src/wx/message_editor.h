#ifndef MESSAGE_EDITOR_H
#define MESSAGE_EDITOR_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif
#include "events.h"
#include "dialog_helper.h"

enum {
  ID_ME_Name = 2000,
  ID_ME_MessageText
};

class SeaMaster;
class Message;
class wxListCtrl;

class MessageEditor : public wxDialog, DialogHelper {
public:
  MessageEditor(wxWindow *parent, Message *message);

private:
  SeaMaster *pm;
  Message *message;
  wxTextCtrl *name_text;
  wxTextCtrl *message_text;

  wxWindow *make_name_panel(wxWindow *parent);

  wxString messages_to_text();

  void save(wxCommandEvent& _);

  wxDECLARE_EVENT_TABLE();
};

#endif /* MESSAGE_EDITOR_H */
