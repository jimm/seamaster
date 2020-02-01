#include "song_list_list_box.h"
#include "../patchmaster.h"
#include "../cursor.h"

SongListListBox::SongListListBox(wxWindow *parent, wxSize size)
  : wxListBox(parent, wxID_ANY, wxDefaultPosition, size),
    initialized(false)
{
}

void SongListListBox::update() {
  PatchMaster *pm = PatchMaster_instance();
  Cursor *cursor = pm->cursor;

  if (!initialized) {
    wxArrayString names;
    for (auto& song_list : pm->song_lists)
      names.Add(song_list->name.c_str());
    InsertItems(names, 0);
    initialized = true;
  }

  int i = 0;
  for (auto& song_list : pm->song_lists) {
    if (song_list == cursor->song_list()) {
      SetSelection(i);
      return;
    }
    ++i;
  }
}

void SongListListBox::jump() {
  PatchMaster *pm = PatchMaster_instance();
  int selection = GetSelection();
  if (selection != wxNOT_FOUND)
    pm->jump_to_song_list(selection);
}
