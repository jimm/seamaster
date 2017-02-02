#include <stdlib.h>
#include "patchmaster.h"
#include "cursor.h"
#include "debug.h"

// ================ allocation ================

PatchMaster::PatchMaster() {
  running = false;
  testing = false;
  all_songs = new SongList((char *)"All Songs"); /* TODO sorted song list */
  song_lists << all_songs;
  cursor = new Cursor(this);
  debug();
}

PatchMaster::~PatchMaster() {
  for (int i = 0; i < inputs.length(); ++i)
    delete inputs[i];
  for (int i = 0; i < outputs.length(); ++i)
    delete outputs[i];
  for (int i = 0; i < all_songs->songs.length(); ++i)
    delete all_songs->songs[i];
  for (int i = 0; i < song_lists.length(); ++i)
    delete song_lists[i];
  for (int i = 0; i < messages.length(); ++i)
    delete messages[i];
  for (int i = 0; i < triggers.length(); ++i)
    delete triggers[i];
}

// ================ running ================

void PatchMaster::start() {
  vdebug("patchmaster_start\n");
  for (int i = 0; i < inputs.length(); ++i)
    inputs[i]->start();
  cursor->init();
  if (cursor->patch() != 0)
    cursor->patch()->start();
  running = true;
}

void PatchMaster::stop() {
  vdebug("patchmaster_stop\n");
  if (cursor->patch() != 0)
    cursor->patch()->stop();
  for (int i = 0; i < inputs.length(); ++i)
    inputs[i]->stop();
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


// ================ vdebugging ================

void PatchMaster::debug() {
  vdebug("patchmaster %p, running %d\n", this, running);
  for (int i = 0; i < inputs.length(); ++i)
    inputs[i]->debug();
  for (int i = 0; i < outputs.length(); ++i)
    outputs[i]->debug();
  for (int i = 0; i < all_songs->songs.length(); ++i)
    all_songs->songs[i]->debug();
  song_lists.debug("song_lists");
  messages.debug("messages");
  triggers.debug("triggers");
  cursor->debug();
}
