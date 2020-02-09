#include "song_list_box.h"
#include "../patchmaster.h"
#include "../cursor.h"

SongListBox::SongListBox(wxWindow *parent, wxWindowID id, wxSize size)
  : wxListBox(parent, id, wxDefaultPosition, size), song_list(nullptr)
{
}

void SongListBox::update() {
  PatchMaster *pm = PatchMaster_instance();
  Cursor *cursor = pm->cursor;
  SongList *curr_song_list = cursor->song_list();

  if (curr_song_list != song_list) {
    Clear();
    if (curr_song_list != nullptr) {
      wxArrayString names;
      for (auto& song : curr_song_list->songs)
        names.Add(song->name.c_str());
      if (!names.IsEmpty())
        InsertItems(names, 0);
    }
    song_list = curr_song_list;
  }

  if (song_list == nullptr)
    return;

  int i = 0;
  for (auto& song : song_list->songs) {
    if (song == cursor->song()) {
      SetSelection(i);
      return;
    }
    ++i;
  }
}

void SongListBox::jump() {
  int selection = GetSelection();
  if (selection != wxNOT_FOUND) {
    PatchMaster *pm = PatchMaster_instance();
    pm->jump_to_song_index(selection);
  }
}
