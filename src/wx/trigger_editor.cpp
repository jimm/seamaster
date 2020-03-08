#include <wx/persist/toplevel.h>
#include "trigger_editor.h"
#include "../patchmaster.h"
#include "../input.h"
#include "../trigger.h"
#include "../formatter.h"

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
  wxString message_str = wxString::Format(
    "%02x %02x %02x",
    Pm_MessageStatus(trigger->trigger_message),
    Pm_MessageData1(trigger->trigger_message),
    Pm_MessageData2(trigger->trigger_message));
  tc_trigger_message = new wxTextCtrl(p, ID_TE_MessageText, message_str);
  sizer->Add(tc_trigger_message);

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
  orig_input = nullptr;
  for (auto &input : pm->inputs) {
    choices.Add(input->name);
    if (count(input->triggers.begin(), input->triggers.end(), trigger) > 0)
      orig_input = input;
  }

  return lc_input = new wxComboBox(
    parent, ID_TE_InputDropdown, orig_input->name, wxDefaultPosition,
    wxDefaultSize, choices, wxCB_READONLY);
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

  return lc_action =  new wxComboBox(
    parent, ID_TE_ActionDropdown, initial_value, wxDefaultPosition,
    wxDefaultSize, choices, wxCB_READONLY);
}

void TriggerEditor::done(wxCommandEvent& event) {
  Input *new_input = pm->inputs[lc_input->GetCurrentSelection()];
  if (new_input != orig_input) {
    orig_input->remove_trigger(trigger);
    new_input->add_trigger(trigger);
  }

  trigger->trigger_message =
    message_from_bytes(tc_trigger_message->GetValue().c_str());

  wxString val = lc_action->GetValue();
  if (val == "Next Song")
    trigger->action = NEXT_SONG;
  else if (val == "Prev Song")
    trigger->action = PREV_SONG;
  else if (val == "Next Patch")
    trigger->action = NEXT_PATCH;
  else if (val == "Prev Patch")
    trigger->action = PREV_PATCH;
  else {
    trigger->action = MESSAGE;
    for (auto &msg : pm->messages) {
      if (msg->name == val) {
        trigger->output_message = msg;
        break;
      }
    }
  }

  wxCommandEvent e(Frame_Refresh, GetId());
  wxPostEvent(GetParent(), e);
  Close();
}
