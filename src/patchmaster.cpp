#include <stdlib.h>
#include "patchmaster.h"
#include "cursor.h"

#define PATCH_STOP  {if (cursor->patch() != nullptr) cursor->patch()->stop();}
#define PATCH_START {if (cursor->patch() != nullptr) cursor->patch()->start();}


static PatchMaster *pm_instance = nullptr;

PatchMaster *PatchMaster_instance() {
  return pm_instance;
}

// ================ allocation ================

PatchMaster::PatchMaster() {
  running = false;
  testing = false;
  all_songs = new SetList(UNDEFINED, (char *)"All Songs");
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
  cursor->init();
  PATCH_START;
  for (auto& in : inputs)
    in->start();
  running = true;
}

void PatchMaster::stop() {
  running = false;
  for (auto& in : inputs)
    in->stop();
  PATCH_STOP;
}

// ================ initialization ================

void PatchMaster::initialize() {
  load_instruments();
  create_songs();
}

void PatchMaster::load_instruments() {
  if (testing)
    return;

  int num_devices = Pm_CountDevices();

  for (int i = 0; i < num_devices; ++i) {
    const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
    if (info->input)
      inputs.push_back(new Input(UNDEFINED, info->name, info->name, i));
    if (info->output)
      outputs.push_back(new Output(UNDEFINED, info->name, info->name, i));
  }
}

void PatchMaster::create_songs() {
  char name[BUFSIZ];

  for (auto& input : inputs) {
    // this input to each individual output
    int output_num = 1;
    for (auto& output : outputs) {
      sprintf(name, "%s -> %s", input->name.c_str(), output->name.c_str());
      Song *song = new Song(UNDEFINED, name);
      all_songs->songs.push_back(song);

      Patch *patch = new Patch(UNDEFINED, name);
      song->patches.push_back(patch);

      Connection *conn = new Connection(UNDEFINED, input, CONNECTION_ALL_CHANNELS,
                                        output, CONNECTION_ALL_CHANNELS);
      patch->connections.push_back(conn);

      ++output_num;
    }

    if (outputs.size() > 1) {
      // one more song: this input to all outputs at once
      sprintf(name, "%s -> all outputs", input->name.c_str());
      Song *song = new Song(UNDEFINED, name);
      all_songs->songs.push_back(song);

      Patch *patch = new Patch(UNDEFINED, name);
      song->patches.push_back(patch);

      for (auto& output : outputs) {
        Connection *conn = new Connection(
          UNDEFINED,
          input, CONNECTION_ALL_CHANNELS,
          output, CONNECTION_ALL_CHANNELS);
        patch->connections.push_back(conn);
      }
    }
  }
}

// ================ movement ================

void PatchMaster::next_patch() {
  PATCH_STOP;
  cursor->next_patch();
  PATCH_START;
}

void PatchMaster::prev_patch() {
  PATCH_STOP;
  cursor->prev_patch();
  PATCH_START;
}

void PatchMaster::next_song() {
  PATCH_STOP;
  cursor->next_song();
  PATCH_START;
}

void PatchMaster::prev_song() {
  PATCH_STOP;
  cursor->prev_song();
  PATCH_START;
}

// ================ going places ================

void PatchMaster::goto_song(string name_regex) {
  PATCH_STOP;
  cursor->goto_song(name_regex);
  PATCH_START;
}

void PatchMaster::goto_set_list(string name_regex) {
  PATCH_STOP;
  cursor->goto_set_list(name_regex);
  PATCH_START;
}

void PatchMaster::jump_to_set_list_index(int i) {
  if (i == cursor->set_list_index)
    return;

  PATCH_STOP;
  cursor->jump_to_set_list_index(i);
  PATCH_START;
}

void PatchMaster::jump_to_song_index(int i) {
  if (i == cursor->song_index)
    return;

  PATCH_STOP;
  cursor->jump_to_song_index(i);
  PATCH_START;
}

void PatchMaster::jump_to_patch_index(int i) {
  if (i == cursor->patch_index)
    return;

  PATCH_STOP;
  cursor->jump_to_patch_index(i);
  PATCH_START;
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

// ================ helpers ================

bool songNameComparator(Song *s1, Song *s2) {
  return s1->name < s2->name;
}

void PatchMaster::sort_all_songs() {
  sort(all_songs->songs.begin(), all_songs->songs.end(), songNameComparator);
}
