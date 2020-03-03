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
  Message *message = new Message("Unnamed Message");
  pm->messages.push_back(message);
  return message;
}

Trigger *Editor::create_trigger(Input *input) {
  Trigger *trigger = new Trigger(Pm_Message(CONTROLLER, 50, 127),
                                 NEXT_PATCH, nullptr);
  input->triggers.push_back(trigger);
  return trigger;
}

Song *Editor::create_song() {
  return create_song(pm->cursor->set_list(), -1);
}

Song *Editor::create_song(SetList *set_list, int position) {
  Song *song = new Song("Unnamed Song");
  // TODO consolidate with Loader::ensure_song_has_patch
  create_patch(song);
  if (position < 0)
    pm->all_songs->songs.push_back(song);
  else {
    vector<Song *> slist = pm->all_songs->songs;
    int i = 0;
    for (ITER(Song) iter = slist.begin(); iter != slist.end(); ++iter) {
      if (i == position) {
        slist.insert(iter, song);
        break;
      }
      ++i;
    }
  }
  pm->sort_all_songs();

  if (set_list != pm->all_songs) {
    // TODO insert song into current set list after current song
  }

  return song;
}

Patch *Editor::create_patch() {
  return create_patch(pm->cursor->song());
}

Patch *Editor::create_patch(Song *song) {
  Patch *p = new Patch("Unnamed Patch");
  song->patches.push_back(p);
  return p;
}

Connection *Editor::create_connection(Input *input, Output *output)
{
  Connection *conn = new Connection(input, CONNECTION_ALL_CHANNELS,
                                    output, CONNECTION_ALL_CHANNELS);
  pm->cursor->patch()->connections.push_back(conn);
  return conn;
}

SetList *Editor::create_set_list() {
  SetList *set_list = new SetList("Unnamed Set List");
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
  for (auto &input : pm->inputs) {
    for (ITER(Trigger) i = input->triggers.begin(); i != input->triggers.end(); ++i)
    {
      if (*i == trigger) {
        input->triggers.erase(i);
        delete trigger;
        return;
      }
    }
  }
}

void Editor::destroy_song(Song *song) {
  if (song == pm->cursor->song())
    pm->next_song();

  // remove song from all set lists first
  for (auto &set_list : pm->set_lists)
    remove_song_from_set_list(song, set_list);
  for (ITER(Song) i = pm->all_songs->songs.begin(); i != pm->all_songs->songs.end(); ++i)
  {
    if (*i == song) {
      pm->all_songs->songs.erase(i);
      delete song;
      return;
    }
  }
}

void Editor::destroy_patch(Song *song, Patch *patch) {
  if (patch == pm->cursor->patch())
    pm->next_patch();

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
