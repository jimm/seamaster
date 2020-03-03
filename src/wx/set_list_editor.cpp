#include <wx/persist/toplevel.h>
#include "set_list_editor.h"
#include "../patchmaster.h"
#include "../set_list.h"

#define FRAME_NAME "seamaster_main_frame"
#define SLE_DONE_BUTTON 1000

wxBEGIN_EVENT_TABLE(SetListEditor, wxDialog)
  EVT_BUTTON(SLE_DONE_BUTTON, SetListEditor::done)
wxEND_EVENT_TABLE()

SetListEditor::SetListEditor(wxWindow *parent, SetList *slist)
  : wxDialog(parent, wxID_ANY, "Set List Editor", wxDefaultPosition, wxSize(480, 500)),
    pm(PatchMaster_instance()), set_list(slist)
{     
  wxPanel *p = new wxPanel(this, wxID_ANY);
  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer *list_sizer = new wxBoxSizer(wxHORIZONTAL);
  list_sizer->Add(make_all_songs_panel(p), wxEXPAND);
  list_sizer->Add(make_set_list_panel(p), wxEXPAND);

  sizer->Add(list_sizer);
  // TODO better button padding
  sizer->Add(new wxButton(this, SLE_DONE_BUTTON, "Done"),
             wxSizerFlags().Right());

  p->SetSizerAndFit(sizer);
  SetClientSize(p->GetSize());
  Show(true);
  wxPersistentRegisterAndRestore(this, FRAME_NAME); // not working?
}

wxWindow *SetListEditor::make_all_songs_panel(wxPanel *parent) {
  wxWindow *retval = make_panel(parent, "All Songs", pm->all_songs,
                                &all_songs_wxlist);
  return retval;
}

wxWindow *SetListEditor::make_set_list_panel(wxPanel *parent) {
  wxWindow *retval = make_panel(parent, set_list->name.c_str(), set_list,
                                &set_list_wxlist);
  return retval;
}

wxWindow *SetListEditor::make_panel(wxPanel *parent, const char * const title,
                                    SetList *slist, wxListBox **list_ptr)
{
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  *list_ptr = new wxListBox(p, wxID_ANY, wxDefaultPosition, wxSize(200, 400),
                            0, nullptr, wxLB_SINGLE);

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(new wxStaticText(p, -1, title), wxSizerFlags().Align(wxALIGN_LEFT));
  sizer->Add(*list_ptr, wxSizerFlags(1).Expand().Border(wxALL));

  if (!slist->songs.empty()) {
    wxArrayString names;
    for (auto& song : slist->songs)
      names.Add(song->name.c_str());
    (*list_ptr)->InsertItems(names, 0);
  }

  p->SetSizerAndFit(sizer);
  return p;
}

void SetListEditor::done(wxCommandEvent& event) {
  Close();
}
