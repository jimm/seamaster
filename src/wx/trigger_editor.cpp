#include <wx/persist/toplevel.h>
#include "trigger_editor.h"
#include "../patchmaster.h"
#include "../input.h"
#include "../trigger.h"

#define FRAME_NAME "seamaster_main_frame"

wxBEGIN_EVENT_TABLE(TriggerEditor, wxDialog)
  EVT_BUTTON(ID_TE_DoneButton, TriggerEditor::done)
wxEND_EVENT_TABLE()

TriggerEditor::TriggerEditor(wxWindow *parent, Trigger *t)
  : wxDialog(parent, wxID_ANY, "Trigger Editor", wxDefaultPosition, wxSize(480, 500)),
  pm(PatchMaster_instance()), trigger(t)
{
  wxPanel *p = new wxPanel(this, wxID_ANY);
  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

  sizer->Add(new wxStaticText(p, wxID_ANY, "Input"));
  sizer->Add(make_input_dropdown(p));

  sizer->Add(new wxStaticText(p, wxID_ANY, "Message"));
  sizer->Add(new wxTextCtrl(p, ID_TE_MessageText));

  sizer->Add(new wxStaticText(p, wxID_ANY, "Action"));
  sizer->Add(make_action_dropdown(p));

  // TODO better button padding
  sizer->Add(new wxButton(this, ID_TE_DoneButton, "Done"),
             wxSizerFlags().Right());

  p->SetSizerAndFit(sizer);
  SetClientSize(p->GetSize());
  Show(true);
  wxPersistentRegisterAndRestore(this, FRAME_NAME); // not working?
}

wxWindow *TriggerEditor::make_input_dropdown(wxPanel *parent) {
  wxArrayString choices;
  Input *curr_input = nullptr;
  for (auto &input : pm->inputs) {
    choices.Add(input->name);
    if (count(input->triggers.begin(), input->triggers.end(), trigger) > 0)
      curr_input = input;
  }

  return new wxComboBox(parent, ID_TE_InputDropdown, curr_input->name,
                        wxDefaultPosition, wxDefaultSize, choices,
                        wxCB_READONLY);
}

wxWindow *TriggerEditor::make_action_dropdown(wxPanel *parent) {
  wxArrayString choices;
  wxString initial_value;

  choices.Add("Next Song");
  choices.Add("Prev Song");
  choices.Add("Next Patch");
  choices.Add("Prev Patch");
  for (auto &message : pm->messages)
    choices.Add(message->name);

  switch (trigger->action) {
  case NEXT_SONG:
    initial_value = "Next Song";
    break;
  case PREV_SONG:
    initial_value = "Prev Song";
    break;
  case NEXT_PATCH:
    initial_value = "Next Patch";
    break;
  case PREV_PATCH:
    initial_value = "Prev Patch";
    break;
  case MESSAGE:
    initial_value = trigger->output_message->name;
    break;
  }

  return new wxComboBox(parent, ID_TE_ActionDropdown, initial_value,
                        wxDefaultPosition, wxDefaultSize, choices,
                        wxCB_READONLY);
}

void TriggerEditor::done(wxCommandEvent& event) {
  wxCommandEvent e(Frame_Refresh, GetId());
  wxPostEvent(GetParent(), e);
  Close();
}
