#include "consts.h"
#include "editor.h"
#include "seamaster.h"
#include "cursor.h"

#define ITER(type) vector<type *>::iterator

Editor::Editor(SeaMaster *sea_master)
  : sm(sea_master ? sea_master : SeaMaster_instance())
{
}

Message *Editor::create_message() {
  return new Message(UNDEFINED_ID, "Unnamed Message");
}

Trigger *Editor::create_trigger(Input *input) {
  return new Trigger(UNDEFINED_ID, TA_NEXT_PATCH, nullptr);
}

Song *Editor::create_song() {
  Song *song = new Song(UNDEFINED_ID, "Unnamed Song");
  // TODO consolidate with Storage::create_default_patch
  Patch *patch = create_patch();
  add_patch(patch, song);
  return song;
}

Patch *Editor::create_patch() {
  return new Patch(UNDEFINED_ID, "Unnamed Patch");
}

Connection *Editor::create_connection(Input *input, Output *output)
{
  return new Connection(UNDEFINED_ID, input, CONNECTION_ALL_CHANNELS,
                        output, CONNECTION_ALL_CHANNELS);
}

SetList *Editor::create_set_list() {
  return new SetList(UNDEFINED_ID, "Unnamed Set List");
}

void Editor::add_message(Message *message) {
  sm->messages.push_back(message);
}

void Editor::add_trigger(Trigger *trigger) {
  sm->triggers.push_back(trigger);
}

void Editor::add_song(Song *song) {
  sm->all_songs->songs.push_back(song);
  sm->sort_all_songs();

  SetList *curr_set_list = sm->cursor->set_list();
  if (curr_set_list == sm->all_songs)
    return;

  vector<Song *> &slist = curr_set_list->songs;
  Song *curr_song = sm->cursor->song();
  if (curr_song == nullptr)
    slist.push_back(song);
  else {
    for (ITER(Song) iter = slist.begin(); iter != slist.end(); ++iter) {
      if (*iter == curr_song) {
        slist.insert(++iter, song);
        break;
      }
    }
  }
}

void Editor::add_patch(Patch *patch) {
  add_patch(patch, sm->cursor->song());
}

void Editor::add_patch(Patch *patch, Song *song) {
  song->patches.push_back(patch);
}

void Editor::add_connection(Connection *connection, Patch *patch)
{
  if (patch != nullptr)
    patch->connections.push_back(connection);
}

void Editor::add_set_list(SetList *set_list) {
  sm->set_lists.push_back(set_list);
}

void Editor::destroy_message(Message *message) {
  for (ITER(Message) i = sm->messages.begin(); i != sm->messages.end(); ++i) {
    if (*i == message) {
      sm->messages.erase(i);
      delete message;
      return;
    }
  }
}

void Editor::destroy_trigger(Trigger *trigger) {
  trigger->remove_from_input();

  for (ITER(Trigger) i = sm->triggers.begin(); i != sm->triggers.end(); ++i) {
    if (*i == trigger) {
      sm->triggers.erase(i);
      delete trigger;
      return;
    }
  }
}

// Returns true if `message` is not used anywhere.
bool Editor::ok_to_destroy_message(Message *message) {
  if (message == nullptr)
    return false;

  for (auto &song : sm->all_songs->songs)
    for (auto &patch : song->patches)
      if (patch->start_message == message || patch->stop_message == message)
        return false;

  for (auto &trigger : sm->triggers)
    if (trigger->output_message == message)
      return false;

  return true;
}

bool Editor::ok_to_destroy_trigger(Trigger *trigger) {
  return true;
}

bool Editor::ok_to_destroy_song(Song *song) {
  return true;
}

bool Editor::ok_to_destroy_patch(Song *song, Patch *patch) {
  return song != nullptr
    && song->patches.size() >= 1;
}

bool Editor::ok_to_destroy_connection(Patch *patch, Connection *connection) {
  return patch != nullptr
    && connection != nullptr
    && patch->connections.size() >= 1;
}

bool Editor::ok_to_destroy_set_list(SetList *set_list) {
  return set_list != nullptr
    && set_list != sm->all_songs;
}

void Editor::destroy_song(Song *song) {
  if (sm->cursor->patch())
    sm->cursor->patch()->stop();
  move_away_from_song(song);

  for (auto &set_list : sm->set_lists)
    remove_song_from_set_list(song, set_list);

  for (ITER(Song) i = sm->all_songs->songs.begin();
       i != sm->all_songs->songs.end();
       ++i)
  {
    if (*i == song) {
      sm->all_songs->songs.erase(i);
      delete song;
      return;                   // only appears once in all_songs list
    }
  }

  if (sm->cursor->patch())
    sm->cursor->patch()->start();
}

// Will not destroy the only patch in a song.
void Editor::destroy_patch(Song *song, Patch *patch) {
  if (song->patches.size() <= 1)
    return;

  if (sm->cursor->patch())
    sm->cursor->patch()->stop();
  move_away_from_patch(song, patch);

  for (ITER(Patch) i = song->patches.begin(); i != song->patches.end(); ++i) {
    if (*i == patch) {
      song->patches.erase(i);
      delete patch;
      break;
    }
  }

  if (sm->cursor->patch())
    sm->cursor->patch()->start();
}

void Editor::destroy_connection(Patch *patch, Connection *connection) {
  if (patch == sm->cursor->patch())
    patch->stop();

  for (ITER(Connection) i = patch->connections.begin(); i != patch->connections.end(); ++i) {
    if (*i == connection) {
      patch->connections.erase(i);
      delete connection;
      break;
    }
  }
  // shouldn't get here, assuming connection is in patch
  if (patch == sm->cursor->patch())
    patch->start();
}

void Editor::destroy_set_list(SetList *set_list) {
  if (set_list == sm->cursor->set_list())
    sm->cursor->goto_set_list("All Songs");

  for (ITER(SetList) i = sm->set_lists.begin(); i != sm->set_lists.end(); ++i) {
    if (*i == set_list) {
      sm->set_lists.erase(i);
      delete set_list;
      return;
    }
  }
}

void Editor::remove_song_from_set_list(Song *song, SetList *set_list) {
  for (ITER(SetList) i = sm->set_lists.begin(); i != sm->set_lists.end(); ++i) {
    SetList *slist = *i;
    if (set_list == slist) {
      for (ITER(Song) j = slist->songs.begin(); j != slist->songs.end(); ++j) {
        if (*j == song) {
          slist->songs.erase(j);
          break;
        }
      }
    }
  }
}

// If `song` is not the current song, does nothing. Else tries to move to
// the next song (see comment though) or, if there isn't one, the prev song.
// If both those fail (this is the only song in the current set list) then
// reinits the cursor.
void Editor::move_away_from_song(Song *song) {
  Cursor *cursor = sm->cursor;

  if (song != cursor->song())
    return;

  if (cursor->has_next_song()) {
    // Don't move to next song because this one will be deleted and the
    // cursor index will point to the next song. Do set the cursor's patch
    // index back to 0.
    cursor->patch_index = 0;
    return;
  }

  if (cursor->has_prev_song()) {
    sm->prev_song();
    return;
  }

  // Nowhere to move. Reini the cursor;
  cursor->init();
}

// If `patch` is not the current patch, does nothing. Else tries to move to
// the next patch or, if there isn't one, the prev patch. If both those fail
// (this is the only patch in the current song) then calls
// move_away_from_song.
void Editor::move_away_from_patch(Song *song, Patch *patch) {
  Cursor *cursor = sm->cursor;

  if (patch != cursor->patch())
    return;

  if (cursor->has_next_patch_in_song()) {
    // Do nothing. We'll remove the patch from the song. The cursor patch
    // index will remain the same, so it will point to the patch that is
    // currently after this one.
    return;
  }

  if (cursor->has_prev_patch_in_song()) {
    sm->prev_patch();
    return;
  }

  move_away_from_song(song);
}
