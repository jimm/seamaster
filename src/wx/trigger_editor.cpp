#include <wx/persist/toplevel.h>
#include "trigger_editor.h"
#include "../patchmaster.h"
#include "../trigger.h"

#define FRAME_NAME "seamaster_main_frame"

wxBEGIN_EVENT_TABLE(TriggerEditor, wxDialog)
  EVT_BUTTON(ID_TE_DoneButton, TriggerEditor::done)
wxEND_EVENT_TABLE()

TriggerEditor::TriggerEditor(wxWindow *parent, Trigger *m)
  : wxDialog(parent, wxID_ANY, "Trigger Editor", wxDefaultPosition, wxSize(480, 500)),
    pm(PatchMaster_instance()), trigger(m)
{
  wxPanel *p = new wxPanel(this, wxID_ANY);
  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

  // TODO better button padding
  sizer->Add(new wxButton(this, ID_TE_DoneButton, "Done"),
             wxSizerFlags().Right());

  p->SetSizerAndFit(sizer);
  SetClientSize(p->GetSize());
  Show(true);
  wxPersistentRegisterAndRestore(this, FRAME_NAME); // not working?
}

void TriggerEditor::done(wxCommandEvent& event) {
  wxCommandEvent e(Frame_Refresh, GetId());
  wxPostEvent(GetParent(), e);
  Close();
}
