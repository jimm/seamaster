#ifndef CONNECTION_EDITOR_H
#define CONNECTION_EDITOR_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif
#include "events.h"

enum {
  ID_CE_DoneButton = 4000,
};

class PatchMaster;
class Connection;
class wxListCtrl;

class ConnectionEditor : public wxDialog {
public:
  ConnectionEditor(wxWindow *parent, Connection *connection);

private:
  PatchMaster *pm;
  Connection *connection;
  wxTextCtrl *name_text;

  void set_name(wxCommandEvent& event);

  void done(wxCommandEvent& event);

  wxDECLARE_EVENT_TABLE();
};

#endif /* CONNECTION_EDITOR_H */
