#include "set_list_box.h"
#include "../patchmaster.h"
#include "../cursor.h"

SetListBox::SetListBox(wxWindow *parent, wxWindowID id, wxSize size)
  : wxListBox(parent, id, wxDefaultPosition, size, 0, nullptr, wxLB_SINGLE),
    set_list(nullptr)
{
}

void SetListBox::update() {
  PatchMaster *pm = PatchMaster_instance();
  Cursor *cursor = pm->cursor;
  SetList *curr_set_list = cursor->set_list();

  Clear();
  if (curr_set_list != nullptr) {
    wxArrayString names;
    for (auto& song : curr_set_list->songs)
      names.Add(song->name.c_str());
    if (!names.IsEmpty())
      InsertItems(names, 0);
  }
  set_list = curr_set_list;

  if (set_list == nullptr)
    return;

  int i = 0;
  for (auto& song : set_list->songs) {
    if (song == cursor->song()) {
      SetSelection(i);
      return;
    }
    ++i;
  }
}

void SetListBox::jump() {
  int selection = GetSelection();
  if (selection != wxNOT_FOUND) {
    PatchMaster *pm = PatchMaster_instance();
    pm->jump_to_song_index(selection);
  }
}
