#include <wx/persist/toplevel.h>
#include "message_editor.h"
#include "../patchmaster.h"
#include "../message.h"

#define FRAME_NAME "seamaster_main_frame"

wxBEGIN_EVENT_TABLE(MessageEditor, wxDialog)
  EVT_TEXT(ID_ME_Name, MessageEditor::set_name)
  EVT_BUTTON(ID_ME_DoneButton, MessageEditor::done)
wxEND_EVENT_TABLE()

MessageEditor::MessageEditor(wxWindow *parent, Message *m)
  : wxDialog(parent, wxID_ANY, "Message Editor", wxDefaultPosition, wxSize(480, 500)),
    pm(PatchMaster_instance()), message(m)
{
  wxPanel *p = new wxPanel(this, wxID_ANY);
  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

  sizer->Add(make_name_panel(p), wxEXPAND);

  // TODO better button padding
  sizer->Add(new wxButton(this, ID_ME_DoneButton, "Done"),
             wxSizerFlags().Right());

  p->SetSizerAndFit(sizer);
  SetClientSize(p->GetSize());
  Show(true);
  wxPersistentRegisterAndRestore(this, FRAME_NAME); // not working?
}

wxWindow *MessageEditor::make_name_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);

  sizer->Add(new wxStaticText(p, wxID_ANY, "Name"));
  name_text = new wxTextCtrl(p, ID_ME_Name);
  sizer->Add(name_text);

  p->SetSizerAndFit(sizer);
  return p;
}

void MessageEditor::set_name(wxCommandEvent& event) {
  message->name = name_text->GetLineText(0);
}

void MessageEditor::done(wxCommandEvent& event) {
  wxCommandEvent e(Frame_Refresh, GetId());
  wxPostEvent(GetParent(), e);
  Close();
}
