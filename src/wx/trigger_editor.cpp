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
  wxSizerFlags label_flags = wxSizerFlags().Border(wxTOP|wxLEFT|wxRIGHT, 10);
  wxSizerFlags field_flags = wxSizerFlags().Border(wxLEFT|wxRIGHT, 10);

  sizer->Add(new wxStaticText(p, wxID_ANY, "Input"), label_flags);
  sizer->Add(make_input_dropdown(p), field_flags);

  sizer->Add(new wxStaticText(p, wxID_ANY, "Message"), label_flags);
  wxString message_str = wxString::Format(
    "%02x %02x %02x",
    Pm_MessageStatus(trigger->trigger_message),
    Pm_MessageData1(trigger->trigger_message),
    Pm_MessageData2(trigger->trigger_message));
  tc_trigger_message = new wxTextCtrl(p, ID_TE_MessageText, message_str);
  sizer->Add(tc_trigger_message, field_flags);

  sizer->Add(new wxStaticText(p, wxID_ANY, "Action"), label_flags);
  sizer->Add(make_action_dropdown(p), field_flags);

  sizer->Add(new wxButton(this, ID_TE_DoneButton, "Done"),
             wxSizerFlags().Right().Border(wxALL, 10));

  p->SetSizerAndFit(sizer);
  SetClientSize(p->GetSize());
  Show(true);
  wxPersistentRegisterAndRestore(this, FRAME_NAME); // not working?
}

wxWindow *TriggerEditor::make_input_dropdown(wxPanel *parent) {
  wxArrayString choices;
  Input *orig_input = trigger->input();
  for (auto &input : pm->inputs)
    choices.Add(input->name);

  return lc_input = new wxComboBox(
    parent, ID_TE_InputDropdown,
    orig_input ? orig_input->name : "",
    wxDefaultPosition,
    wxDefaultSize, choices, wxCB_READONLY);
}

wxWindow *TriggerEditor::make_action_dropdown(wxPanel *parent) {
  wxArrayString choices;
  wxString initial_value;

  choices.Add("Next Song");
  choices.Add("Prev Song");
  choices.Add("Next Patch");
  choices.Add("Prev Patch");
  choices.Add("Panic");
  choices.Add("Super Panic");
  for (auto &message : pm->messages)
    choices.Add(message->name);

  switch (trigger->action) {
  case TA_NEXT_SONG:
    initial_value = "Next Song";
    break;
  case TA_PREV_SONG:
    initial_value = "Prev Song";
    break;
  case TA_NEXT_PATCH:
    initial_value = "Next Patch";
    break;
  case TA_PREV_PATCH:
    initial_value = "Prev Patch";
    break;
  case TA_PANIC:
    initial_value = "Panic";
    break;
  case TA_SUPER_PANIC:
    initial_value = "Super Panic";
    break;
  case TA_MESSAGE:
    initial_value = trigger->output_message->name;
    break;
  }

  return lc_action =  new wxComboBox(
    parent, ID_TE_ActionDropdown, initial_value, wxDefaultPosition,
    wxDefaultSize, choices, wxCB_READONLY);
}

void TriggerEditor::done(wxCommandEvent& event) {
  Input *new_input = pm->inputs[lc_input->GetCurrentSelection()];

  // TODO handle key code instead of input
  PmMessage msg = message_from_bytes(tc_trigger_message->GetValue().c_str());
  trigger->set_trigger_message(new_input, msg);

  wxString val = lc_action->GetValue();
  if (val == "Next Song")
    trigger->action = TA_NEXT_SONG;
  else if (val == "Prev Song")
    trigger->action = TA_PREV_SONG;
  else if (val == "Next Patch")
    trigger->action = TA_NEXT_PATCH;
  else if (val == "Prev Patch")
    trigger->action = TA_PREV_PATCH;
  else if (val == "Panic")
    trigger->action = TA_PANIC;
  else if (val == "Super Panic")
    trigger->action = TA_SUPER_PANIC;
  else {
    trigger->action = TA_MESSAGE;
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
