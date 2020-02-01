#include "song_box.h"
#include "../patchmaster.h"
#include "../cursor.h"

SongBox::SongBox(wxWindow *parent, wxSize size)
  : wxListBox(parent, wxID_ANY, wxDefaultPosition, size)
{
}

void SongBox::update() {
  PatchMaster *pm = PatchMaster_instance();
  Cursor *cursor = pm->cursor;
  Song *curr_song = cursor->song();

  if (curr_song != song) {
    Clear();
    if (curr_song != nullptr) {
      wxArrayString names;
      for (auto& patch : curr_song->patches)
        names.Add(patch->name.c_str());
      InsertItems(names, 0);
    }
    song = curr_song;
  }

  if (song == nullptr)
    return;

  int i = 0;
  for (auto& patch : song->patches) {
    if (patch == cursor->patch()) {
      SetSelection(i);
      return;
    }
    ++i;
  }
}

void SongBox::jump() {
  PatchMaster *pm = PatchMaster_instance();
  int selection = GetSelection();
  if (selection != wxNOT_FOUND)
    pm->jump_to_patch(selection);
}
