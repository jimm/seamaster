#include "song_box.h"
#include "../patchmaster.h"
#include "../cursor.h"

SongBox::SongBox(wxWindow *parent, wxWindowID id, wxSize size)
  : FrameListBox(parent, id, wxDefaultPosition, size, wxLB_SINGLE),
    song(nullptr)
{
}

void SongBox::update() {
  PatchMaster *pm = PatchMaster_instance();
  Cursor *cursor = pm->cursor;
  Song *curr_song = cursor->song();

  Clear();
  if (curr_song != nullptr) {
    wxArrayString names;
    for (auto& patch : curr_song->patches)
      names.Add(patch->name.c_str());
    if (!names.empty())
      InsertItems(names, 0);
  }
  song = curr_song;

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
  int selection = GetSelection();
  if (selection != wxNOT_FOUND) {
    PatchMaster *pm = PatchMaster_instance();
    pm->jump_to_patch_index(selection);
  }
}
