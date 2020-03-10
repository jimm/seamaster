#include <wx/persist/toplevel.h>
#include "connection_editor.h"
#include "controller_mappings.h"
#include "controller_editor.h"
#include "events.h"
#include "../patchmaster.h"
#include "../connection.h"
#include "../formatter.h"

#define FRAME_NAME "seamaster_main_frame"

wxDEFINE_EVENT(Connection_Refresh, wxCommandEvent);

wxBEGIN_EVENT_TABLE(ConnectionEditor, wxDialog)
  EVT_BUTTON(ID_CE_AddControllerMapping, ConnectionEditor::add_controller_mapping)
  EVT_BUTTON(ID_CE_DelControllerMapping, ConnectionEditor::del_controller_mapping)
  EVT_BUTTON(ID_CE_DoneButton, ConnectionEditor::done)
  EVT_LIST_ITEM_ACTIVATED(ID_CE_ControllerMappings, ConnectionEditor::edit_controller_mapping)
  EVT_LIST_ITEM_SELECTED(ID_CE_ControllerMappings, ConnectionEditor::update_buttons)
  EVT_LIST_ITEM_DESELECTED(ID_CE_ControllerMappings, ConnectionEditor::update_buttons)
  EVT_COMMAND(wxID_ANY, Connection_Refresh, ConnectionEditor::update)
wxEND_EVENT_TABLE()

ConnectionEditor::ConnectionEditor(wxWindow *parent, Connection *c)
  : wxDialog(parent, wxID_ANY, "Connection Editor", wxDefaultPosition, wxSize(480, 500)),
    pm(PatchMaster_instance()), connection(c)
{
  wxPanel *p = new wxPanel(this, wxID_ANY);
  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

  sizer->Add(make_input_panel(p));
  sizer->Add(make_output_panel(p));
  sizer->Add(make_program_panel(p));
  sizer->Add(make_zone_panel(p));
  sizer->Add(make_xpose_panel(p));
  sizer->Add(make_sysex_panel(p));
  sizer->Add(make_cc_maps_panel(p));

  sizer->Add(new wxButton(this, ID_CE_DoneButton, "Done"),
             wxSizerFlags().Right().Border(wxALL, 10));

  p->SetSizerAndFit(sizer);
  SetClientSize(p->GetSize());
  update();
  Show(true);
  wxPersistentRegisterAndRestore(this, FRAME_NAME); // not working?
}

wxWindow *ConnectionEditor::make_input_panel(wxPanel *parent) {
  return make_instrument_panel(
    parent, ID_CE_InputDropdown, ID_CE_InputChannel,
    &cb_input, &cb_input_chan,
    reinterpret_cast<vector<Instrument *> &>(pm->inputs),
    connection->input, connection->input_chan);
}

wxWindow *ConnectionEditor::make_output_panel(wxPanel *parent) {
  return make_instrument_panel(
    parent, ID_CE_OutputDropdown, ID_CE_OutputChannel,
    &cb_output, &cb_output_chan,
    reinterpret_cast<vector<Instrument *> &>(pm->outputs),
    connection->output, connection->output_chan);
}

wxWindow *ConnectionEditor::make_instrument_panel(
  wxPanel *parent, wxWindowID inst_id, wxWindowID chan_id,
  wxComboBox **instrument_combo_ptr, wxComboBox **chan_combo_ptr,
  vector<Instrument *> &instruments, Instrument *curr_instrument,
  int curr_chan)
{
  wxArrayString choices;
  wxString curr_output;
  for (auto &instrument : instruments) {
    choices.Add(instrument->name);
    if (instrument == curr_instrument)
      curr_output = instrument->name;
  }

  wxPanel *p = new wxPanel(parent, wxID_ANY);
  wxBoxSizer *outer_sizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *field_sizer = new wxBoxSizer(wxHORIZONTAL);

  *instrument_combo_ptr = new wxComboBox(
    p, inst_id, curr_output, wxDefaultPosition, wxDefaultSize, choices,
    wxCB_READONLY);
  field_sizer->Add(*instrument_combo_ptr);
  *chan_combo_ptr = make_channel_dropdown(p, chan_id, curr_chan, "All Channels");
  field_sizer->Add(*chan_combo_ptr);

  outer_sizer->Add(new wxStaticText(p, wxID_ANY, "Output"));
  outer_sizer->Add(field_sizer);

  p->SetSizerAndFit(outer_sizer);
  return p;
}

wxComboBox *ConnectionEditor::make_channel_dropdown(
  wxPanel *parent, wxWindowID id, int curr_val, const char * const first_choice)
{
  wxArrayString choices;
  choices.Add(first_choice);
  for (int i = 1; i <= 16; ++i)
    choices.Add(wxString::Format("%d", i));
  wxString curr_choice = curr_val == CONNECTION_ALL_CHANNELS
    ? wxString(first_choice)
    : wxString::Format("%d", curr_val + 1);

  return new wxComboBox(parent, id, curr_choice,
                        wxDefaultPosition, wxDefaultSize, choices,
                        wxCB_READONLY);
}

wxWindow *ConnectionEditor::make_program_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  wxBoxSizer *outer_sizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *field_sizer = new wxBoxSizer(wxHORIZONTAL);

  wxString val = connection->prog.bank_msb >= 0
    ? wxString::Format("%d", connection->prog.bank_msb) : "";
  field_sizer->Add(new wxStaticText(p, wxID_ANY, "Bank MSB"));
  tc_bank_msb = new wxTextCtrl(p, ID_CE_BankMSB, val);
  field_sizer->Add(tc_bank_msb);

  val = connection->prog.bank_lsb >= 0
    ? wxString::Format("%d", connection->prog.bank_lsb) : "";
  field_sizer->Add(new wxStaticText(p, wxID_ANY, "Bank LSB"));
  tc_bank_lsb = new wxTextCtrl(p, ID_CE_BankLSB, val);
  field_sizer->Add(tc_bank_lsb);

  val = connection->prog.prog >= 0
    ? wxString::Format("%d", connection->prog.prog) : "";
  field_sizer->Add(new wxStaticText(p, wxID_ANY, "PChg"));
  tc_prog = new wxTextCtrl(p, ID_CE_Program, val);
  field_sizer->Add(tc_prog);

  outer_sizer->Add(new wxStaticText(p, wxID_ANY, "Program Change"));
  outer_sizer->Add(field_sizer);

  p->SetSizerAndFit(outer_sizer);
  return p;
}

wxWindow *ConnectionEditor::make_zone_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  wxBoxSizer *outer_sizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *field_sizer = new wxBoxSizer(wxHORIZONTAL);

  char buf[BUFSIZ];
  note_num_to_name(connection->zone.low, buf);
  wxString val(buf);
  field_sizer->Add(new wxStaticText(p, wxID_ANY, "Low"));
  tc_zone_low = new wxTextCtrl(p, ID_CE_ZoneLow, val);
  field_sizer->Add(tc_zone_low);

  note_num_to_name(connection->zone.high, buf);
  val = buf;
  field_sizer->Add(new wxStaticText(p, wxID_ANY, "High"));
  tc_zone_high = new wxTextCtrl(p, ID_CE_ZoneHigh, val);
  field_sizer->Add(tc_zone_high);

  outer_sizer->Add(new wxStaticText(p, wxID_ANY, "Zone"));
  outer_sizer->Add(field_sizer);

  p->SetSizerAndFit(outer_sizer);
  return p;
}

wxWindow *ConnectionEditor::make_xpose_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  wxBoxSizer *outer_sizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *field_sizer = new wxBoxSizer(wxHORIZONTAL);

  wxString val = wxString::Format("%d", connection->xpose);
  tc_xpose = new wxTextCtrl(p, ID_CE_Transpose, val);
  field_sizer->Add(tc_xpose);

  outer_sizer->Add(new wxStaticText(p, wxID_ANY, "Transpose"));
  outer_sizer->Add(field_sizer);

  p->SetSizerAndFit(outer_sizer);
  return p;
}

wxWindow *ConnectionEditor::make_sysex_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  wxBoxSizer *outer_sizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *field_sizer = new wxBoxSizer(wxHORIZONTAL);

  cb_sysex = new wxCheckBox(
    p, ID_CE_PassThroughSysex, "Pass Through Sysex Messages");
  field_sizer->Add(cb_sysex);

  outer_sizer->Add(new wxStaticText(p, wxID_ANY, "Sysex"));
  outer_sizer->Add(field_sizer);

  p->SetSizerAndFit(outer_sizer);
  return p;
}

wxWindow *ConnectionEditor::make_cc_maps_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  wxBoxSizer *outer_sizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *button_sizer = new wxBoxSizer(wxHORIZONTAL);

  lc_cc_mappings = new ControllerMappings(p, ID_CE_ControllerMappings,
                                          connection);

  b_add_ccmap = new wxButton(p, ID_CE_AddControllerMapping, " + ");
  button_sizer->Add(b_add_ccmap, wxSizerFlags().Left());

  b_del_ccmap = new wxButton(p, ID_CE_DelControllerMapping, " - ");
  button_sizer->Add(b_del_ccmap, wxSizerFlags().Left());

  outer_sizer->Add(lc_cc_mappings);
  outer_sizer->Add(button_sizer);

  p->SetSizerAndFit(outer_sizer);
  return p;
}

Instrument *ConnectionEditor::input_from_instrument_list(
  wxComboBox *list, vector<Instrument *>& instruments)
{
  return instruments[list->GetCurrentSelection()];
}

int ConnectionEditor::channel_from_channel_list(wxComboBox *list) {
  int n = list->GetCurrentSelection();
  return n == 0 ? CONNECTION_ALL_CHANNELS : (n - 1);
}

int ConnectionEditor::int_or_undefined_from_field(wxTextCtrl *field) {
  wxString val = field->GetValue();
  if (val.empty())
    return UNDEFINED;
  return int_from_chars((const char *)val);
}

void ConnectionEditor::edit_controller_mapping(wxListEvent& event) {
  int controller_num = selected_cc_map_index();
  if (controller_num != wxNOT_FOUND)
    edit_controller_mapping(connection->cc_maps[controller_num]);
}

void ConnectionEditor::edit_controller_mapping(Controller *controller) {
  if (controller != nullptr)
    new ControllerEditor(this, connection, controller);
}

void ConnectionEditor::update_buttons() {
  bool has_free_cc_map_slot = false;
  for (int i = 0; i < 128 && !has_free_cc_map_slot; ++i)
    if (connection->cc_maps[i] == nullptr)
      has_free_cc_map_slot = true;

  b_add_ccmap->Enable(has_free_cc_map_slot);
  b_del_ccmap->Enable(selected_cc_map_index() != wxNOT_FOUND);
}

void ConnectionEditor::add_controller_mapping(wxCommandEvent& event) {
  int cc_num = -1;
  for (int i = 0; i < 128; ++i) {
    if (connection->cc_maps[i] == nullptr) {
      cc_num = i;
      break;
    }
  }
  if (cc_num == -1)
    return;

  Controller *cc = new Controller(UNDEFINED, cc_num);
  connection->cc_maps[cc_num] = cc;
  update();
  edit_controller_mapping(cc);
}

void ConnectionEditor::del_controller_mapping(wxCommandEvent& event) {
  int controller_num = selected_cc_map_index();
  if (controller_num == wxNOT_FOUND)
    return;

  connection->remove_cc_num(controller_num);
  update();
}

void ConnectionEditor::update() {
  lc_cc_mappings->update();
  update_buttons();
}

void ConnectionEditor::done(wxCommandEvent& event) {
  connection->input = pm->inputs[cb_input->GetCurrentSelection()];
  int n = cb_input->GetCurrentSelection();
  connection->input_chan = (n == 0 ? CONNECTION_ALL_CHANNELS : n - 1);

  connection->output = pm->outputs[cb_output->GetCurrentSelection()];
  n = cb_output->GetCurrentSelection();
  connection->output_chan = (n == 0 ? CONNECTION_ALL_CHANNELS : n - 1);

  connection->prog.bank_msb = int_or_undefined_from_field(tc_bank_msb);
  connection->prog.bank_lsb = int_or_undefined_from_field(tc_bank_lsb);
  connection->prog.prog = int_or_undefined_from_field(tc_prog);
  connection->zone.low = note_name_to_num(tc_zone_low->GetValue());
  connection->zone.high = note_name_to_num(tc_zone_high->GetValue());
  connection->xpose = int_from_chars(tc_xpose->GetValue());
  connection->pass_through_sysex = cb_sysex->IsChecked();

  // Don't need to update cc_maps because that's done on the fly

  wxCommandEvent e(Frame_Refresh, GetId());
  wxPostEvent(GetParent(), e);
  Close();
}

long ConnectionEditor::selected_cc_map_index() {
  return lc_cc_mappings->GetNextItem(wxNOT_FOUND, wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);
}
