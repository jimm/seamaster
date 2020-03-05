#include <wx/persist/toplevel.h>
#include "connection_editor.h"
#include "../patchmaster.h"
#include "../connection.h"

#define FRAME_NAME "seamaster_main_frame"

wxBEGIN_EVENT_TABLE(ConnectionEditor, wxDialog)
  EVT_BUTTON(ID_CE_DoneButton, ConnectionEditor::done)
wxEND_EVENT_TABLE()

ConnectionEditor::ConnectionEditor(wxWindow *parent, Connection *m)
  : wxDialog(parent, wxID_ANY, "Connection Editor", wxDefaultPosition, wxSize(480, 500)),
    pm(PatchMaster_instance()), connection(m)
{
  wxPanel *p = new wxPanel(this, wxID_ANY);
  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

  // TODO better button padding
  sizer->Add(new wxButton(this, ID_CE_DoneButton, "Done"),
             wxSizerFlags().Right());

  p->SetSizerAndFit(sizer);
  SetClientSize(p->GetSize());
  Show(true);
  wxPersistentRegisterAndRestore(this, FRAME_NAME); // not working?
}

void ConnectionEditor::done(wxCommandEvent& event) {
  wxCommandEvent e(Frame_Refresh, GetId());
  wxPostEvent(GetParent(), e);
  Close();
}
