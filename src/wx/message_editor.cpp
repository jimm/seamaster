#include <wx/persist/toplevel.h>
#include "message_editor.h"
#include "../patchmaster.h"
#include "../message.h"
#include "../formatter.h"

#define WIDTH 200
#define HEIGHT 300
#define FRAME_NAME "seamaster_main_frame"

wxBEGIN_EVENT_TABLE(MessageEditor, wxDialog)
  EVT_BUTTON(ID_ME_DoneButton, MessageEditor::done)
wxEND_EVENT_TABLE()

MessageEditor::MessageEditor(wxWindow *parent, Message *m)
  : wxDialog(parent, wxID_ANY, "Message Editor", wxDefaultPosition, wxSize(480, 500)),
    pm(PatchMaster_instance()), message(m)
{
  wxPanel *p = new wxPanel(this, wxID_ANY);
  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  wxSizerFlags panel_flags = wxSizerFlags().Border(wxTOP|wxLEFT|wxRIGHT, 10);

  sizer->Add(make_name_panel(p), panel_flags);
  message_text = new wxTextCtrl(p, ID_ME_MessageText, messages_to_text(),
                                wxDefaultPosition, wxSize(WIDTH, HEIGHT),
                                wxTE_MULTILINE);
  sizer->Add(message_text,
             wxSizerFlags().Expand().Border(wxTOP|wxLEFT|wxRIGHT, 10));

  sizer->Add(new wxButton(this, ID_ME_DoneButton, "Done"),
             wxSizerFlags().Right().Border(wxALL, 10));

  p->SetSizerAndFit(sizer);
  SetClientSize(p->GetSize());
  Show(true);
  wxPersistentRegisterAndRestore(this, FRAME_NAME); // not working?
}

wxWindow *MessageEditor::make_name_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  wxSizerFlags center_flags =
    wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL);

  sizer->Add(new wxStaticText(p, wxID_ANY, "Name"), center_flags);
  name_text = new wxTextCtrl(p, ID_ME_Name, message->name, wxDefaultPosition);
  sizer->Add(name_text, center_flags);

  p->SetSizerAndFit(sizer);
  return p;
}

wxString MessageEditor::messages_to_text() {
  wxString text = "";
  for (auto pm_msg : message->messages) {
    unsigned char status = Pm_MessageStatus(pm_msg);
    unsigned char switch_status = status < 0xf0 ? (status & 0xf0) : status;
    switch (switch_status & 0xf0) {
    case NOTE_OFF: case NOTE_ON: case POLY_PRESSURE: case CONTROLLER:
    case PITCH_BEND: case SONG_POINTER:
      text += "0x";
      text += byte_to_hex(status);
      text += " 0x";
      text += byte_to_hex(Pm_MessageData1(pm_msg));
      text += " 0x";
      text += byte_to_hex(Pm_MessageData2(pm_msg));
      text += "\n";
      break;
    case PROGRAM_CHANGE: case CHANNEL_PRESSURE: case SONG_SELECT:
      text += "0x";
      text += byte_to_hex(status);
      text += " 0x";
      text += byte_to_hex(Pm_MessageData1(pm_msg));
      text += "\n";
      break;
    default:
      text += "0x";
      text += byte_to_hex(status);
      text += "\n";
      break;
    }
  }
  return text;
}

void MessageEditor::done(wxCommandEvent& event) {
  // extract data from text edit widget
  message->name = name_text->GetLineText(0);
  wxString text = message_text->GetValue();
  char text_buf[BUFSIZ];
  vector<char *>line_ptrs;

  // Need to be careful about memory use here. `strok` (used here and by
  // `message_from_bytes`) munges the string passed in, and it is not thread
  // safe. We make sure each call is run in isolation (first splitting text
  // into lines, then parsing each line) and runs on a char array.
  strcpy(text_buf, text.c_str());
  message->messages.clear();
  for (char *line = strtok(text_buf, "\n");
       line != nullptr;
       line = strtok(nullptr, "\n"))
    line_ptrs.push_back(line);

  for (auto p : line_ptrs) {
    char line_buf[BUFSIZ];
    strcpy(line_buf, p);
    message->messages.push_back(message_from_bytes(line_buf));
  }

  wxCommandEvent e(Frame_Refresh, GetId());
  wxPostEvent(GetParent(), e);
  Close();
}
