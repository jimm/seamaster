#include "consts.h"
#include "editor.h"
#include "patchmaster.h"
#include "cursor.h"

#define ITER(type) vector<type *>::iterator

Editor::Editor(PatchMaster *pmaster)
  : pm(pmaster ? pmaster : PatchMaster_instance())
{
}

void Editor::create_message() {
  pm->messages.push_back(new Message("Unnamed Message"));
}

void Editor::create_trigger(Input *input) {
  input->triggers.push_back(new Trigger(Pm_Message(CONTROLLER, 50, 127),
                                        NEXT_PATCH, nullptr));
}

void Editor::create_song() {
  Song *s = new Song("Unnamed Song");
  // TODO consolidate with Loader::ensure_song_has_patch
  Patch *p = new Patch("Unnamed Patch");
  s->patches.push_back(p);
  pm->all_songs->songs.push_back(s);
  pm->sort_all_songs();

  if (pm->cursor->set_list() != pm->all_songs) {
    // TODO insert song into current set list after current song
  }
}

void Editor::create_patch() {
  Patch *p = new Patch("Unnamed Patch");
  pm->cursor->song()->patches.push_back(p);
}

void Editor::create_connection(Input *input, Output *output)
{
  Connection *conn = new Connection(input, CONNECTION_ALL_CHANNELS,
                                    output, CONNECTION_ALL_CHANNELS);
  pm->cursor->patch()->connections.push_back(conn);
}

void Editor::create_set_list() {
  pm->set_lists.push_back(new SetList("Unnamed Set List"));
}

void Editor::destroy_message(Message *m) {
  for (ITER(Message) i = pm->messages.begin(); i != pm->messages.end(); ++i) {
    if (*i == m) {
      pm->messages.erase(i);
      delete m;
      return;
    }
  }
}

void Editor::destroy_trigger(Trigger *t) {
  for (auto &input : pm->inputs) {
    for (ITER(Trigger) i = input->triggers.begin(); i != input->triggers.end(); ++i)
    {
      if (*i == t) {
        input->triggers.erase(i);
        delete t;
        return;
      }
    }
  }
}

void Editor::destroy_song(Song *s) {
  if (s == pm->cursor->song())
    pm->next_song();

  // remove song from all set lists first
  for (auto &sl : pm->set_lists)
    remove_song_from_set_list(s, sl);
  for (ITER(Song) i = pm->all_songs->songs.begin(); i != pm->all_songs->songs.end(); ++i)
  {
    if (*i == s) {
      pm->all_songs->songs.erase(i);
      delete s;
      return;
    }
  }
}

void Editor::destroy_patch(Song *s, Patch *p) {
  if (p == pm->cursor->patch())
    pm->next_patch();

  for (ITER(Patch) i = s->patches.begin(); i != s->patches.end(); ++i) {
    if (*i == p) {
      s->patches.erase(i);
      delete p;
      return;
    }
  }
}

void Editor::destroy_connection(Patch *p, Connection *c) {
  if (p == pm->cursor->patch())
    p->stop();

  for (ITER(Connection) i = p->connections.begin(); i != p->connections.end(); ++i) {
    if (*i == c) {
      p->connections.erase(i);
      delete c;
      if (p == pm->cursor->patch())
        p->start();
      return;
    }
  }
  // shouldn't get here, assuming c is in p
  if (p == pm->cursor->patch())
    p->start();
}

void Editor::destroy_set_list(SetList *sl) {
  if (sl == pm->cursor->set_list())
    pm->cursor->goto_set_list("All Songs");

  for (ITER(SetList) i = pm->set_lists.begin(); i != pm->set_lists.end(); ++i) {
    if (*i == sl) {
      pm->set_lists.erase(i);
      delete sl;
      return;
    }
  }
}

void Editor::remove_song_from_set_list(Song *song, SetList *set_list) {
  for (ITER(SetList) i = pm->set_lists.begin(); i != pm->set_lists.end(); ++i) {
    SetList *sl = *i;
    if (sl == set_list) {
      for (ITER(Song) j = sl->songs.begin(); j != sl->songs.end(); ++j) {
        if (*j == song)
          sl->songs.erase(j);
      }
    }
  }
}
