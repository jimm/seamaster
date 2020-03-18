#include <wx/persist/toplevel.h>
#include "set_list_editor.h"
#include "../patchmaster.h"
#include "../set_list.h"

#define FRAME_NAME "seamaster_main_frame"

wxBEGIN_EVENT_TABLE(SetListEditor, wxDialog)
  EVT_TEXT(ID_SLE_Name, SetListEditor::set_name)

  EVT_LISTBOX(ID_SLE_AllSongs, SetListEditor::all_songs_selection)
  EVT_LISTBOX_DCLICK(ID_SLE_AllSongs, SetListEditor::add_song)

  EVT_LISTBOX(ID_SLE_SetList, SetListEditor::set_list_selection)
  EVT_LISTBOX_DCLICK(ID_SLE_SetList, SetListEditor::remove_song)

  EVT_BUTTON(ID_SLE_AddButton, SetListEditor::add_song)
  EVT_BUTTON(ID_SLE_RemoveButton, SetListEditor::remove_song)

  EVT_BUTTON(ID_SLE_DoneButton, SetListEditor::done)
wxEND_EVENT_TABLE()

SetListEditor::SetListEditor(wxWindow *parent, SetList *slist)
  : wxDialog(parent, wxID_ANY, "Set List Editor", wxDefaultPosition, wxSize(480, 500)),
    pm(PatchMaster_instance()), set_list(slist)
{     
  wxPanel *p = new wxPanel(this, wxID_ANY);
  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

  sizer->Add(make_name_panel(p), wxEXPAND);

  wxBoxSizer *list_sizer = new wxBoxSizer(wxHORIZONTAL);
  list_sizer->Add(make_all_songs_panel(p), wxEXPAND);
  list_sizer->Add(make_buttons(p), wxEXPAND);
  list_sizer->Add(make_set_list_panel(p), wxEXPAND);

  sizer->Add(list_sizer);
  sizer->Add(new wxButton(this, ID_SLE_DoneButton, "Done"),
             wxSizerFlags().Right().Border());

  p->SetSizerAndFit(sizer);
  SetClientSize(p->GetSize());
  Show(true);
  wxPersistentRegisterAndRestore(this, FRAME_NAME); // not working?
}

wxWindow *SetListEditor::make_name_panel(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);

  sizer->Add(new wxStaticText(p, wxID_ANY, "Name"));
  name_text = new wxTextCtrl(p, ID_SLE_Name);
  sizer->Add(name_text);

  p->SetSizerAndFit(sizer);
  return p;
}

wxWindow *SetListEditor::make_all_songs_panel(wxPanel *parent) {
  wxWindow *retval = make_panel(parent, ID_SLE_AllSongs, "All Songs", pm->all_songs,
                                &all_songs_wxlist);
  return retval;
}

wxWindow *SetListEditor::make_buttons(wxPanel *parent) {
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  wxBoxSizer *button_sizer = new wxBoxSizer(wxVERTICAL);

  add_button = new wxButton(p, ID_SLE_AddButton, "->");
  remove_button = new wxButton(p, ID_SLE_RemoveButton, "X");

  add_button->Disable();
  remove_button->Disable();

  button_sizer->Add(add_button);
  button_sizer->Add(remove_button);

  p->SetSizerAndFit(button_sizer);
  return p;
}

wxWindow *SetListEditor::make_set_list_panel(wxPanel *parent) {
  wxWindow *retval = make_panel(parent, ID_SLE_SetList, set_list->name.c_str(), set_list,
                                &set_list_wxlist);
  return retval;
}

wxWindow *SetListEditor::make_panel(wxPanel *parent, wxWindowID id,
                                    const char * const title, SetList *slist,
                                    wxListBox **list_ptr)
{
  wxPanel *p = new wxPanel(parent, wxID_ANY);
  *list_ptr = new wxListBox(p, id, wxDefaultPosition, wxSize(200, 400), 0,
                            nullptr, wxLB_SINGLE);

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(new wxStaticText(p, wxID_ANY, title), wxSizerFlags().Align(wxALIGN_LEFT));
  sizer->Add(*list_ptr, wxSizerFlags(1).Expand().Border());

  update(*list_ptr, slist);

  p->SetSizerAndFit(sizer);
  return p;
}

void SetListEditor::set_name(wxCommandEvent& event) {
  set_list->name = name_text->GetLineText(0);
}

void SetListEditor::all_songs_selection(wxCommandEvent& event) {
  add_button->Enable(all_songs_wxlist->GetSelection() != wxNOT_FOUND);
}

void SetListEditor::set_list_selection(wxCommandEvent& event) {
  remove_button->Enable(set_list_wxlist->GetSelection() != wxNOT_FOUND);
}

void SetListEditor::add_song(wxCommandEvent& event) {
  int all_songs_index = all_songs_wxlist->GetSelection();
  if (all_songs_index == wxNOT_FOUND)
    return;
  Song *song = pm->all_songs->songs[all_songs_index];

  int set_list_index = set_list_wxlist->GetSelection();
  if (set_list_index == wxNOT_FOUND
      || set_list_index == set_list->songs.size() - 1)
    set_list->songs.push_back(song);
  else {
    vector<Song *>::iterator iter = set_list->songs.begin();
    iter += set_list_index + 1;
    set_list->songs.insert(iter, song);
  }
  update(set_list_wxlist, set_list);
}

void SetListEditor::remove_song(wxCommandEvent& event) {
  int set_list_index = set_list_wxlist->GetSelection();
  if (set_list_index == wxNOT_FOUND)
    return;

  vector<Song *>::iterator iter = set_list->songs.begin();
  iter += set_list_index;
  set_list->songs.erase(iter);

  update(set_list_wxlist, set_list);
}

void SetListEditor::update(wxListBox *list_box, SetList *slist) {
  list_box->Clear();
  if (slist->songs.empty())
    return;

  wxArrayString names;
  for (auto& song : slist->songs)
    names.Add(song->name.c_str());
  if (!names.empty())
      list_box->InsertItems(names, 0);
}

void SetListEditor::done(wxCommandEvent& event) {
  wxCommandEvent e(Frame_Refresh, GetId());
  wxPostEvent(GetParent(), e);
  Close();
}
