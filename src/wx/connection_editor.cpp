#include <wx/persist/toplevel.h>
#include "connection_editor.h"
#include "../patchmaster.h"
#include "../connection.h"
#include "../formatter.h"

#define FRAME_NAME "seamaster_main_frame"

wxBEGIN_EVENT_TABLE(ConnectionEditor, wxDialog)
  EVT_BUTTON(ID_CE_DoneButton, ConnectionEditor::done)
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

  // TODO better button padding
  sizer->Add(new wxButton(this, ID_CE_DoneButton, "Done"),
             wxSizerFlags().Right());

  p->SetSizerAndFit(sizer);
  SetClientSize(p->GetSize());
  Show(true);
  wxPersistentRegisterAndRestore(this, FRAME_NAME); // not working?
}

wxWindow *ConnectionEditor::make_input_panel(wxPanel *parent) {
  return make_instrument_panel(
    parent, ID_CE_InputDropdown, ID_CE_InputChannel,
    reinterpret_cast<vector<Instrument *> &>(pm->inputs),
    connection->input, connection->input_chan);
}

wxWindow *ConnectionEditor::make_output_panel(wxPanel *parent) {
  return make_instrument_panel(
    parent, ID_CE_OutputDropdown, ID_CE_OutputChannel,
    reinterpret_cast<vector<Instrument *> &>(pm->outputs),
    connection->output, connection->output_chan);
}

wxWindow *ConnectionEditor::make_instrument_panel(
  wxPanel *parent, wxWindowID inst_id, wxWindowID chan_id,
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

  field_sizer->Add(new wxComboBox(
                     p, inst_id, curr_output,
                     wxDefaultPosition, wxDefaultSize, choices,
                     wxCB_READONLY));
  field_sizer->Add(make_channel_dropdown(
                     p, chan_id, connection->output_chan,
                     "All Channels"));

  outer_sizer->Add(new wxStaticText(p, wxID_ANY, "Output"));
  outer_sizer->Add(field_sizer);

  p->SetSizerAndFit(outer_sizer);
  return p;
}

wxWindow *ConnectionEditor::make_channel_dropdown(
  wxPanel *parent, wxWindowID id, int curr_val, const char * const first_choice)
{
  wxArrayString choices;
  wxString currChoice = curr_val == CONNECTION_ALL_CHANNELS
    ? wxString(first_choice)
    : wxString::Format("%d", curr_val + 1);

  choices.Add(first_choice);
  for (int i = 1; i <= 16; ++i)
    choices.Add(wxString::Format("%d", i));
  return new wxComboBox(parent, id, currChoice,
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
  field_sizer->Add(new wxTextCtrl(p, ID_CE_BankMSB, val));

  val = connection->prog.bank_lsb >= 0
    ? wxString::Format("%d", connection->prog.bank_lsb) : "";
  field_sizer->Add(new wxStaticText(p, wxID_ANY, "Bank LSB"));
  field_sizer->Add(new wxTextCtrl(p, ID_CE_BankLSB, val));

  val = connection->prog.prog >= 0
    ? wxString::Format("%d", connection->prog.prog) : "";
  field_sizer->Add(new wxStaticText(p, wxID_ANY, "PChg"));
  field_sizer->Add(new wxTextCtrl(p, ID_CE_Program, val));

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
  field_sizer->Add(new wxTextCtrl(p, ID_CE_ZoneLow, val));

  note_num_to_name(connection->zone.high, buf);
  val = buf;
  field_sizer->Add(new wxStaticText(p, wxID_ANY, "High"));
  field_sizer->Add(new wxTextCtrl(p, ID_CE_ZoneHigh, val));

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
  field_sizer->Add(new wxTextCtrl(p, ID_CE_Transpose, val));

  outer_sizer->Add(new wxStaticText(p, wxID_ANY, "Transpose"));
  outer_sizer->Add(field_sizer);

  p->SetSizerAndFit(outer_sizer);
  return p;
}

wxWindow *ConnectionEditor::make_sysex_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  wxBoxSizer *outer_sizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *field_sizer = new wxBoxSizer(wxHORIZONTAL);

  field_sizer->Add(new wxCheckBox(p, ID_CE_PassThroughSysex, "Pass Through Sysex Messages"));

  outer_sizer->Add(new wxStaticText(p, wxID_ANY, "Sysex"));
  outer_sizer->Add(field_sizer);

  p->SetSizerAndFit(outer_sizer);
  return p;
}

void ConnectionEditor::done(wxCommandEvent& event) {
  // TODO copy data to connection

  wxCommandEvent e(Frame_Refresh, GetId());
  wxPostEvent(GetParent(), e);
  Close();
}
