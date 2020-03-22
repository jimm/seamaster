#include <wx/persist/toplevel.h>
#include "patch_editor.h"
#include "events.h"
#include "../patchmaster.h"
#include "../patch.h"

#define FRAME_NAME "seamaster_main_frame"

wxBEGIN_EVENT_TABLE(PatchEditor, wxDialog)
  EVT_BUTTON(ID_PE_DoneButton, PatchEditor::done)
wxEND_EVENT_TABLE()

PatchEditor::PatchEditor(wxWindow *parent, Patch *patch_ptr)
  : wxDialog(parent, wxID_ANY, "Patch Editor", wxDefaultPosition, wxSize(480, 500)),
    patch(patch_ptr)
{
  wxPanel *p = new wxPanel(this, wxID_ANY);
  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

  wxSizerFlags panel_flags = wxSizerFlags().Border(wxTOP|wxLEFT|wxRIGHT);
  sizer->Add(make_name_panel(p), panel_flags);
  sizer->Add(make_start_panel(p), panel_flags);
  sizer->Add(make_stop_panel(p), panel_flags);

  sizer->Add(new wxButton(this, ID_PE_DoneButton, "Done"),
             wxSizerFlags().Right().Border());

  p->SetSizerAndFit(sizer);
  SetClientSize(p->GetSize());
  Show(true);
  wxPersistentRegisterAndRestore(this, FRAME_NAME); // not working?
}

wxWindow *PatchEditor::make_name_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  wxSizerFlags center_flags =
    wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL);

  sizer->Add(new wxStaticText(p, wxID_ANY, "Name"), center_flags);
  name_text = new wxTextCtrl(p, ID_PE_Name, patch->name, wxDefaultPosition);
  sizer->Add(name_text, center_flags);

  p->SetSizerAndFit(sizer);
  return p;
}

wxWindow *PatchEditor::make_start_panel(wxPanel *parent) {
  return make_message_panel(
    parent, ID_PE_StartMessageDropdown, "Start", patch->start_message,
    &cb_start_message);
}

wxWindow *PatchEditor::make_stop_panel(wxPanel *parent) {
  return make_message_panel(
    parent, ID_PE_StopMessageDropdown, "Stop", patch->stop_message,
    &cb_stop_message);
}

wxWindow *PatchEditor::make_message_panel(
  wxPanel *parent, wxWindowID id, const char * const title,
  Message *curr_message, wxComboBox **combo_ptr)
{
  wxArrayString choices;
  choices.Add("(No Message)");
  wxString curr_choice;
  for (auto &message : PatchMaster_instance()->messages) {
    choices.Add(message->name);
    if (message == curr_message)
      curr_choice = message->name;
  }

  wxPanel *p = new wxPanel(parent, wxID_ANY);
  wxBoxSizer *outer_sizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *field_sizer = new wxBoxSizer(wxHORIZONTAL);

  *combo_ptr = new wxComboBox(
    p, id, curr_choice, wxDefaultPosition, wxDefaultSize, choices,
    wxCB_READONLY);
  field_sizer->Add(*combo_ptr);

  outer_sizer->Add(new wxStaticText(p, wxID_ANY, title));
  outer_sizer->Add(field_sizer);

  p->SetSizerAndFit(outer_sizer);
  return p;
}

void PatchEditor::done(wxCommandEvent& event) {
  PatchMaster *pm = PatchMaster_instance();

  // extract data from text edit widget
  patch->name = name_text->GetLineText(0);

  int index = cb_start_message->GetCurrentSelection();
  if (index == wxNOT_FOUND || index != 0)
    patch->start_message = nullptr;
  else
    patch->start_message = pm->messages[index-1];

  index = cb_stop_message->GetCurrentSelection();
  if (index == wxNOT_FOUND || index != 0)
    patch->stop_message = nullptr;
  else
    patch->stop_message = pm->messages[index-1];

  wxCommandEvent e(Frame_Refresh, GetId());
  wxPostEvent(GetParent(), e);
  Close();
}
