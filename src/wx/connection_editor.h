#ifndef CONNECTION_EDITOR_H
#define CONNECTION_EDITOR_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif
#include "events.h"

using namespace std;

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
  ID_CE_ControllerMappings,
  ID_CE_AddControllerMapping,
  ID_CE_DelControllerMapping,
  ID_CE_DoneButton
};

class PatchMaster;
class Instrument;
class Connection;
class Controller;
class wxListCtrl;
class wxListEvent;

class ConnectionEditor : public wxDialog {
public:
  ConnectionEditor(wxWindow *parent, Connection *connection);

private:
  PatchMaster *pm;
  Connection *connection;
  wxComboBox *lc_input;
  wxComboBox *lc_input_chan;
  wxComboBox *lc_output;
  wxComboBox *lc_output_chan;
  wxTextCtrl *tc_bank_msb;
  wxTextCtrl *tc_bank_lsb;
  wxTextCtrl *tc_prog;
  wxTextCtrl *tc_zone_low;
  wxTextCtrl *tc_zone_high;
  wxTextCtrl *tc_xpose;
  wxCheckBox *cb_sysex;
  wxListCtrl *lc_cc_mappings;
  wxButton *b_add_ccmap;
  wxButton *b_del_ccmap;

  wxWindow *make_input_panel(wxPanel *parent);
  wxWindow *make_output_panel(wxPanel *parent);
  wxComboBox *make_channel_dropdown(
    wxPanel *parent, wxWindowID id, int curr_val,
    const char * const first_choice);
  wxWindow *make_program_panel(wxPanel *parent);
  wxWindow *make_zone_panel(wxPanel *parent);
  wxWindow *make_xpose_panel(wxPanel *parent);
  wxWindow *make_sysex_panel(wxPanel *parent);
  wxWindow *make_cc_maps_panel(wxPanel *parent);

  wxWindow *make_instrument_panel(
    wxPanel *parent, wxWindowID inst_id, wxWindowID chan_id,
    wxComboBox **instrument_combo_ptr, wxComboBox **chan_combo_ptr,
    vector<Instrument *> &instruments, Instrument *curr_instrument,
    int curr_chan);

  Instrument *input_from_instrument_list(
    wxComboBox *list, vector<Instrument *> &instruments);
  int channel_from_channel_list(wxComboBox *list);
  int int_or_undefined_from_field(wxTextCtrl *field);

  void edit_controller_mapping(wxListEvent& event);
  void edit_controller_mapping(Controller *controller);

  void update_buttons(wxListEvent& event) { update_buttons(); }
  void update_buttons();

  void add_controller_mapping(wxCommandEvent& event);
  void del_controller_mapping(wxCommandEvent& event);
  void done(wxCommandEvent& event);

  long selected_cc_map_index();

  wxDECLARE_EVENT_TABLE();
};

#endif /* CONNECTION_EDITOR_H */
