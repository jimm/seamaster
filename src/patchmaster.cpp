#include <stdlib.h>
#include "patchmaster.h"
#include "cursor.h"

static PatchMaster *pm_instance;

PatchMaster *PatchMaster_instance() {
  return pm_instance;
}

// ================ allocation ================

PatchMaster::PatchMaster() {
  running = false;
  testing = false;
  all_songs = new SongList((char *)"All Songs"); /* TODO sorted song list */
  song_lists.push_back(all_songs);
  cursor = new Cursor(this);
  pm_instance = this;
}

PatchMaster::~PatchMaster() {
  if (pm_instance == this)
    pm_instance = 0;

  for (vector<Input *>::iterator i = inputs.begin(); i != inputs.end(); ++i)
    delete *i;
  for (vector<Output *>::iterator i = outputs.begin(); i != outputs.end(); ++i)
    delete *i;
  for (vector<Song *>::iterator i = all_songs->songs.begin(); i != all_songs->songs.end(); ++i)
    delete *i;
  for (vector<SongList *>::iterator i = song_lists.begin(); i != song_lists.end(); ++i)
    delete *i;
  for (vector<Message *>::iterator i = messages.begin(); i != messages.end(); ++i)
    delete *i;
}

// ================ running ================

void PatchMaster::start() {
  for (vector<Input *>::iterator i = inputs.begin(); i != inputs.end(); ++i)
    (*i)->start();
  cursor->init();
  if (cursor->patch() != 0)
    cursor->patch()->start();
  running = true;
}

void PatchMaster::stop() {
  if (cursor->patch() != 0)
    cursor->patch()->stop();
  for (vector<Input *>::iterator i = inputs.begin(); i != inputs.end(); ++i)
    (*i)->stop();
  running = false;
}

// ================ movement ================

void PatchMaster::next_patch() {
  if (cursor->patch() != 0)
    cursor->patch()->stop();
  cursor->next_patch();
  if (cursor->patch() != 0)
    cursor->patch()->start();
}

void PatchMaster::prev_patch() {
  if (cursor->patch() != 0)
    cursor->patch()->stop();
  cursor->prev_patch();
  if (cursor->patch() != 0)
    cursor->patch()->start();
}

void PatchMaster::next_song() {
  if (cursor->patch() != 0)
    cursor->patch()->stop();
  cursor->next_song();
  if (cursor->patch() != 0)
    cursor->patch()->start();
}

void PatchMaster::prev_song() {
  if (cursor->patch() != 0)
    cursor->patch()->stop();
  cursor->prev_song();
  if (cursor->patch() != 0)
    cursor->patch()->start();
}

// ================ going places ================

void PatchMaster::goto_song(string name_regex) {
  if (cursor->patch() != 0)
    cursor->patch()->stop();
  cursor->goto_song(name_regex);
  if (cursor->patch() != 0)
    cursor->patch()->start();
}

void PatchMaster::goto_song_list(string name_regex) {
  if (cursor->patch() != 0)
    cursor->patch()->stop();
  cursor->goto_song_list(name_regex);
  if (cursor->patch() != 0)
    cursor->patch()->start();
}

// ================ doing things ================

void PatchMaster::panic(bool send_notes_off) {
  PmEvent buf[128];

  memset(buf, 0, 128 * sizeof(PmEvent));
  if (send_notes_off) {
    for (int i = 0; i < 16; ++i) {
      for (int j = 0; j < 128; ++j)
        buf[j].message = Pm_Message(NOTE_OFF + i, j, 0);
      for (vector<Output *>::iterator o = outputs.begin(); o != outputs.end(); ++o)
        (*o)->write(buf, 128);
    }
  }
  else {
    for (int i = 0; i < 16; ++i)
      buf[i].message = Pm_Message(CONTROLLER + i, CM_ALL_NOTES_OFF, 0);
    for (vector<Output *>::iterator o = outputs.begin(); o != outputs.end(); ++o)
      (*o)->write(buf, 16);
  }
}
