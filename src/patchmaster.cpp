#include <stdlib.h>
#include "patchmaster.h"
#include "cursor.h"
#include "debug.h"

// ================ allocation ================

PatchMaster::PatchMaster() {
  running = false;
  testing = false;
  inputs = list_new();
  outputs = list_new();
  all_songs = new SongList((char *)"All Songs"); /* TODO sorted song list */
  song_lists = list_new();
  list_append(song_lists, all_songs);
  messages = list_new();
  triggers = list_new();
  cursor = new Cursor(this);
  debug();
}

PatchMaster::~PatchMaster() {
  for (int i = 0; i < list_length(inputs); ++i)
    delete (Input *)list_at(inputs, i);
  list_free(inputs, 0);
  for (int i = 0; i < list_length(outputs); ++i)
    delete (Output *)list_at(outputs, i);
  list_free(outputs, 0);
  for (int i = 0; i < list_length(all_songs->songs); ++i)
    delete (Song *)list_at(all_songs->songs, i);
  for (int i = 0; i < list_length(song_lists); ++i)
    delete (SongList *)list_at(song_lists, i);
  list_free(song_lists, 0);
  for (int i = 0; i < list_length(messages); ++i)
    delete (Message *)list_at(messages, i);
  list_free(messages, 0);
  for (int i = 0; i < list_length(triggers); ++i)
    delete (Trigger *)list_at(triggers, i);
  list_free(triggers, 0);
}

// ================ running ================

void PatchMaster::start() {
  vdebug("patchmaster_start\n");
  for (int i = 0; i < list_length(inputs); ++i)
    ((Input *)list_at(inputs, i))->start();
  cursor->init();
  if (cursor->patch() != 0)
    cursor->patch()->start();
  running = true;
}

void PatchMaster::stop() {
  vdebug("patchmaster_stop\n");
  if (cursor->patch() != 0)
    cursor->patch()->stop();
  for (int i = 0; i < list_length(inputs); ++i)
    ((Input *)list_at(inputs, i))->stop();
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

// ================ vdebugging ================

void PatchMaster::debug() {
  vdebug("patchmaster %p, running %d\n", this, running);
  for (int i = 0; i < list_length(inputs); ++i)
    ((Input *)list_at(inputs, i))->debug();
  for (int i = 0; i < list_length(outputs); ++i)
    ((Output *)list_at(outputs, i))->debug();
  for (int i = 0; i < list_length(all_songs->songs); ++i)
    ((Song *)list_at(all_songs->songs, i))->debug();
  list_debug(song_lists, "song_lists");
  list_debug(messages, "messages");
  list_debug(triggers, "triggers");
  cursor->debug();
}
