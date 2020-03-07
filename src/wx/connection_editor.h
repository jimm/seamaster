#ifndef CONNECTION_EDITOR_H
#define CONNECTION_EDITOR_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif
#include "events.h"

enum {
  ID_CE_InputDropdown = 4000,
  ID_CE_InputChannel,
  ID_CE_OutputDropdown,
  ID_CE_OutputChannel,
  ID_CE_BankMSB,
  ID_CE_BankLSB,
  ID_CE_Program,
  ID_CE_ZoneLow,
  ID_CE_ZoneHigh,
  ID_CE_Transpose,
  ID_CE_PassThroughSysex,
  ID_CE_DoneButton
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

  wxWindow *make_input_panel(wxPanel *parent);
  wxWindow *make_output_panel(wxPanel *parent);
  wxWindow *make_channel_dropdown(wxPanel *parent, wxWindowID id,
                                  int curr_val,
                                  const char * const first_choice);
  wxWindow *make_program_panel(wxPanel *parent);
  wxWindow *make_zone_panel(wxPanel *parent);
  wxWindow *make_xpose_panel(wxPanel *parent);
  wxWindow *make_sysex_panel(wxPanel *parent);

  void done(wxCommandEvent& event);

  wxDECLARE_EVENT_TABLE();
};

#endif /* CONNECTION_EDITOR_H */
