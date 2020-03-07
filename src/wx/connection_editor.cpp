#include <wx/persist/toplevel.h>
#include "connection_editor.h"
#include "../patchmaster.h"
#include "../connection.h"

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
  wxArrayString choices;
  Input *curr_input = nullptr;
  for (auto &input : pm->inputs) {
    choices.Add(input->name);
    if (input == connection->input)
      curr_input = input;
  }

  wxPanel *p = new wxPanel(parent, wxID_ANY);
  wxBoxSizer *outer_sizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *field_sizer = new wxBoxSizer(wxHORIZONTAL);

  field_sizer->Add(new wxComboBox(
                     parent, ID_CE_InputDropdown, curr_input->name,
                     wxDefaultPosition, wxDefaultSize, choices,
                     wxCB_READONLY));
  field_sizer->Add(make_channel_dropdown(
                     p, ID_CE_InputChannel, connection->input_chan,
                     "All Channels"));

  outer_sizer->Add(new wxStaticText(p, wxID_ANY, "Input"));
  outer_sizer->Add(field_sizer);

  p->SetSizerAndFit(outer_sizer);
  return p;
}

wxWindow *ConnectionEditor::make_output_panel(wxPanel *parent) {
  wxArrayString choices;
  Output *curr_output = nullptr;
  for (auto &output : pm->outputs) {
    choices.Add(output->name);
    if (output == connection->output)
      curr_output = output;
  }

  wxPanel *p = new wxPanel(parent, wxID_ANY);
  wxBoxSizer *outer_sizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *field_sizer = new wxBoxSizer(wxHORIZONTAL);

  field_sizer->Add(new wxComboBox(
                     parent, ID_CE_OutputDropdown, curr_output->name,
                     wxDefaultPosition, wxDefaultSize, choices,
                     wxCB_READONLY));
  field_sizer->Add(make_channel_dropdown(
                     p, ID_CE_OutputChannel, connection->output_chan,
                     "Same Channel"));

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

  outer_sizer->Add(new wxStaticText(p, wxID_ANY, "Program Change"));
  outer_sizer->Add(field_sizer);

  p->SetSizerAndFit(outer_sizer);
  return p;
}

wxWindow *ConnectionEditor::make_zone_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  wxBoxSizer *outer_sizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *field_sizer = new wxBoxSizer(wxHORIZONTAL);

  outer_sizer->Add(new wxStaticText(p, wxID_ANY, "Zone"));
  outer_sizer->Add(field_sizer);

  p->SetSizerAndFit(outer_sizer);
  return p;
}

wxWindow *ConnectionEditor::make_xpose_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  wxBoxSizer *outer_sizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *field_sizer = new wxBoxSizer(wxHORIZONTAL);

  outer_sizer->Add(new wxStaticText(p, wxID_ANY, "Transpose"));
  outer_sizer->Add(field_sizer);

  p->SetSizerAndFit(outer_sizer);
  return p;
}

wxWindow *ConnectionEditor::make_sysex_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  wxBoxSizer *outer_sizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *field_sizer = new wxBoxSizer(wxHORIZONTAL);

  outer_sizer->Add(new wxStaticText(p, wxID_ANY, "Output"));
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
