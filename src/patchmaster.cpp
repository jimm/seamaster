#include <stdlib.h>
#include "patchmaster.h"
#include "cursor.h"

static PatchMaster *pm_instance = nullptr;

PatchMaster *PatchMaster_instance() {
  return pm_instance;
}

// ================ allocation ================

PatchMaster::PatchMaster() {
  running = false;
  testing = false;
  all_songs = new SetList((char *)"All Songs"); /* TODO sorted set list */
  set_lists.push_back(all_songs);
  cursor = new Cursor(this);
  pm_instance = this;
}

PatchMaster::~PatchMaster() {
  if (pm_instance == this)
    pm_instance = 0;

  for (auto& in : inputs)
    delete in;
  for (auto& out : outputs)
    delete out;
  for (auto& song : all_songs->songs)
    delete song;
  for (auto& set_list : set_lists)
    delete set_list;
  for (auto& msg : messages)
    delete msg;
}

// ================ running ================

void PatchMaster::start() {
  for (auto& in : inputs)
    in->start();
  cursor->init();
  if (cursor->patch() != nullptr)
    cursor->patch()->start();
  running = true;
}

void PatchMaster::stop() {
  if (cursor->patch() != nullptr)
    cursor->patch()->stop();
  for (auto& in : inputs)
    in->stop();
  running = false;
}

// ================ movement ================

void PatchMaster::next_patch() {
  if (cursor->patch() != nullptr)
    cursor->patch()->stop();
  cursor->next_patch();
  if (cursor->patch() != nullptr)
    cursor->patch()->start();
}

void PatchMaster::prev_patch() {
  if (cursor->patch() != nullptr)
    cursor->patch()->stop();
  cursor->prev_patch();
  if (cursor->patch() != nullptr)
    cursor->patch()->start();
}

void PatchMaster::next_song() {
  if (cursor->patch() != nullptr)
    cursor->patch()->stop();
  cursor->next_song();
  if (cursor->patch() != nullptr)
    cursor->patch()->start();
}

void PatchMaster::prev_song() {
  if (cursor->patch() != nullptr)
    cursor->patch()->stop();
  cursor->prev_song();
  if (cursor->patch() != nullptr)
    cursor->patch()->start();
}

// ================ going places ================

void PatchMaster::goto_song(string name_regex) {
  if (cursor->patch() != nullptr)
    cursor->patch()->stop();
  cursor->goto_song(name_regex);
  if (cursor->patch() != nullptr)
    cursor->patch()->start();
}

void PatchMaster::goto_set_list(string name_regex) {
  if (cursor->patch() != nullptr)
    cursor->patch()->stop();
  cursor->goto_set_list(name_regex);
  if (cursor->patch() != nullptr)
    cursor->patch()->start();
}

void PatchMaster::jump_to_set_list_index(int i) {
  if (i == cursor->set_list_index)
    return;

  if (cursor->patch() != nullptr)
    cursor->patch()->stop();
  cursor->jump_to_set_list_index(i);
  if (cursor->patch() != nullptr)
    cursor->patch()->start();
}

void PatchMaster::jump_to_song_index(int i) {
  if (i == cursor->song_index)
    return;

  if (cursor->patch() != nullptr)
    cursor->patch()->stop();
  cursor->jump_to_song_index(i);
  if (cursor->patch() != nullptr)
    cursor->patch()->start();
}

void PatchMaster::jump_to_patch_index(int i) {
  if (i == cursor->patch_index)
    return;

  if (cursor->patch() != nullptr)
    cursor->patch()->stop();
  cursor->jump_to_patch_index(i);
  if (cursor->patch() != nullptr)
    cursor->patch()->start();
}

// ================ doing things ================

void PatchMaster::panic(bool send_notes_off) {
  PmEvent buf[128];

  memset(buf, 0, 128 * sizeof(PmEvent));
  if (send_notes_off) {
    for (int i = 0; i < 128; ++i)
      buf[i].timestamp = 0;
    for (int i = 0; i < 16; ++i) {
      for (int j = 0; j < 128; ++j)
        buf[j].message = Pm_Message(NOTE_OFF + i, j, 0);
      for (auto& out : outputs)
        out->write(buf, 128);
    }
  }
  else {
    for (int i = 0; i < 16; ++i) {
      buf[i].message = Pm_Message(CONTROLLER + i, CM_ALL_NOTES_OFF, 0);
      buf[i].timestamp = 0;
    }
    for (auto& out : outputs)
      out->write(buf, 16);
  }
}
