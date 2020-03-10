#ifndef CONTROLLER_EDITOR_H
#define CONTROLLER_EDITOR_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
 #include <wx/wx.h>
#endif
#include "events.h"

using namespace std;

enum {
  ID_CMAP_CCNumber = 5000,
  ID_CMAP_TranslatedNumber,
  ID_CMAP_Min,
  ID_CMAP_Max,
  ID_CMAP_Filtered,
  ID_CMAP_DoneButton
};

class PatchMaster;
class Instrument;
class Connection;
class Controller;
class wxListCtrl;
class wxListEvent;

class ControllerEditor : public wxDialog {
public:
  ControllerEditor(wxWindow *parent, Connection *connection, Controller *controller);

private:
  Connection *connection;
  Controller *controller;
  int orig_cc_num;
  wxComboBox *cb_cc_number;
  wxComboBox *cb_xlated_number;
  wxTextCtrl *tc_min;
  wxTextCtrl *tc_max;
  wxCheckBox *cb_filtered;

  wxWindow *make_numbers_panel(wxPanel *parent);
  wxWindow *make_minmax_panel(wxPanel *parent);
  wxWindow *make_filtered_panel(wxPanel *parent);
  wxComboBox *make_cc_number_dropdown(wxPanel *parent, wxWindowID id,
                                      int curr_val, bool filter_out_existing);

  void done(wxCommandEvent& event);

  wxDECLARE_EVENT_TABLE();
};

#endif /* CONTROLLER_EDITOR_H */
