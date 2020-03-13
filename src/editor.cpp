#include "consts.h"
#include "editor.h"
#include "patchmaster.h"
#include "cursor.h"

#define ITER(type) vector<type *>::iterator

Editor::Editor(PatchMaster *pmaster)
  : pm(pmaster ? pmaster : PatchMaster_instance())
{
}

Message *Editor::create_message() {
  Message *message = new Message(UNDEFINED_ID, "Unnamed Message");
  pm->messages.push_back(message);
  return message;
}

Trigger *Editor::create_trigger(Input *input) {
  Trigger *trigger = new Trigger(UNDEFINED_ID, TA_NEXT_PATCH, nullptr);
  pm->triggers.push_back(trigger);
  trigger->set_trigger_message(input, Pm_Message(CONTROLLER, 50, 127));
  return trigger;
}

Song *Editor::create_song() {
  Song *song = new Song(UNDEFINED_ID, "Unnamed Song");
  // TODO consolidate with Loader::ensure_song_has_patch
  create_patch(song);
  pm->all_songs->songs.push_back(song);
  pm->sort_all_songs();

  SetList *curr_set_list = pm->cursor->set_list();
  if (curr_set_list == pm->all_songs)
    return song;

  vector<Song *> slist = curr_set_list->songs;
  Song *curr_song = pm->cursor->song();
  for (ITER(Song) iter = slist.begin(); iter != slist.end(); ++iter) {
    if (*iter == curr_song) {
      slist.insert(++iter, song);
      break;
    }
  }

  return song;
}

Patch *Editor::create_patch() {
  return create_patch(pm->cursor->song());
}

Patch *Editor::create_patch(Song *song) {
  Patch *p = new Patch(UNDEFINED_ID, "Unnamed Patch");
  song->patches.push_back(p);
  return p;
}

Connection *Editor::create_connection(Patch *patch, Input *input, Output *output)
{
  if (patch == nullptr)
    return nullptr;

  Connection *conn = new Connection(UNDEFINED_ID, input, CONNECTION_ALL_CHANNELS,
                                    output, CONNECTION_ALL_CHANNELS);
  patch->connections.push_back(conn);
  return conn;
}

SetList *Editor::create_set_list() {
  SetList *set_list = new SetList(UNDEFINED_ID, "Unnamed Set List");
  pm->set_lists.push_back(set_list);
  return set_list;
}

void Editor::destroy_message(Message *message) {
  for (ITER(Message) i = pm->messages.begin(); i != pm->messages.end(); ++i) {
    if (*i == message) {
      pm->messages.erase(i);
      delete message;
      return;
    }
  }
}

void Editor::destroy_trigger(Trigger *trigger) {
  for (auto &input : pm->inputs)
    for (ITER(Trigger) i = input->triggers.begin(); i != input->triggers.end(); ++i)
      if (*i == trigger)
        input->triggers.erase(i);

  for (ITER(Trigger) i = pm->triggers.begin(); i != pm->triggers.end(); ++i) {
    if (*i == trigger) {
      pm->triggers.erase(i);
      delete trigger;
      return;
    }
  }
}

// Returns true if `message` is not used anywhere.
bool Editor::ok_to_destroy_message(Message *message) {
  if (message == nullptr)
    return false;

  for (auto &song : pm->all_songs->songs)
    for (auto &patch : song->patches)
      if (patch->start_message == message || patch->stop_message == message)
        return false;

  for (auto &trigger : pm->triggers)
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
    && set_list != pm->all_songs;
}

void Editor::destroy_song(Song *song) {
  move_away_from_song(song);

  for (auto &set_list : pm->set_lists)
    remove_song_from_set_list(song, set_list);

  for (ITER(Song) i = pm->all_songs->songs.begin();
       i != pm->all_songs->songs.end();
       ++i)
  {
    if (*i == song) {
      pm->all_songs->songs.erase(i);
      delete song;
      return;                   // only appears once in all_songs list
    }
  }
}

// Will not destroy the only patch in a song.
void Editor::destroy_patch(Song *song, Patch *patch) {
  if (song->patches.size() <= 1)
    return;

  move_away_from_patch(song, patch);
  for (ITER(Patch) i = song->patches.begin(); i != song->patches.end(); ++i) {
    if (*i == patch) {
      song->patches.erase(i);
      delete patch;
      return;
    }
  }
}

void Editor::destroy_connection(Patch *patch, Connection *connection) {
  if (patch == pm->cursor->patch())
    patch->stop();

  for (ITER(Connection) i = patch->connections.begin(); i != patch->connections.end(); ++i) {
    if (*i == connection) {
      patch->connections.erase(i);
      delete connection;
      if (patch == pm->cursor->patch())
        patch->start();
      return;
    }
  }
  // shouldn't get here, assuming connection is in patch
  if (patch == pm->cursor->patch())
    patch->start();
}

void Editor::destroy_set_list(SetList *set_list) {
  if (set_list == pm->cursor->set_list())
    pm->cursor->goto_set_list("All Songs");

  for (ITER(SetList) i = pm->set_lists.begin(); i != pm->set_lists.end(); ++i) {
    if (*i == set_list) {
      pm->set_lists.erase(i);
      delete set_list;
      return;
    }
  }
}

void Editor::remove_song_from_set_list(Song *song, SetList *set_list) {
  for (ITER(SetList) i = pm->set_lists.begin(); i != pm->set_lists.end(); ++i) {
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
// the next song or, if there isn't one, the prev song. If both those fail
// (this is the only song in the current set list) then stops the current
// patch and reinits the cursor.
void Editor::move_away_from_song(Song *song) {
  Cursor *c = pm->cursor;

  if (song != c->song())
    return;

  if (c->has_next_song()) {
    pm->next_song();
    return;
  }

  if (c->has_prev_song()) {
    pm->prev_song();
    return;
  }

  if (c->patch() != nullptr)
    c->patch()->stop();
  c->init();
}

// If `patch` is not the current patch, does nothing. Else tries to move to
// the next patch or, if there isn't one, the prev patch. If both those fail
// (this is the only patch in the current song) then calls
// move_away_from_song.
void Editor::move_away_from_patch(Song *song, Patch *patch) {
  Cursor *c = pm->cursor;

  if (patch != c->patch())
    return;

  if (c->has_next_patch_in_song()) {
    pm->next_patch();
    return;
  }

  if (c->has_prev_patch_in_song()) {
    pm->prev_patch();
    return;
  }

  move_away_from_song(song);
}
