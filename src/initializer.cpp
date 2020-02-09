#include <portmidi.h>
#include "initializer.h"
#include "loader.h"

#define INIT_FILE_PATH "./seamaster.md"

void Initializer::initialize() {
  Loader loader;

  PatchMaster *old_pm = PatchMaster_instance();
  PatchMaster *pm = new PatchMaster();    // side-effect: PatchMaster static instance set
  load_instruments(pm);
  create_songs(pm);
  if (old_pm != nullptr) {
    old_pm->stop();
    delete old_pm;
  }
  pm->start();                  // initializes cursor
}

void Initializer::load_instruments(PatchMaster *pm) {
  int num_devices = Pm_CountDevices();
  int input_num = 1, output_num = 1;
  char sym[8];

  for (int i = 0; i < num_devices; ++i) {
    const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
    if (info->input) {
      sprintf(sym, "in%d\n", input_num);
      pm->inputs.push_back(new Input(sym, info->name, info->name, i));
      ++input_num;
    }
    if (info->output) {
      sprintf(sym, "out%d\n", output_num);
      pm->outputs.push_back(new Output(sym, info->name, info->name, i));
      ++output_num;
    }
  }
}

void Initializer::create_songs(PatchMaster *pm) {
  int input_num = 1;
  char name[32];

  for (auto& input : pm->inputs) {
    // this input to each individual output
    int output_num = 1;
    for (auto& output : pm->outputs) {
      sprintf(name, "in%d -> out%d", input_num, output_num);

      Song *song = new Song(name);
      pm->all_songs->songs.push_back(song);

      Patch *patch = new Patch(name);
      song->patches.push_back(patch);

      Connection *conn = new Connection(input, CONNECTION_ALL_CHANNELS,
                                        output, CONNECTION_ALL_CHANNELS);
      patch->connections.push_back(conn);

      ++output_num;
    }

    // one more song: this input to all outputs at once
    sprintf(name, "in%d -> all outputs", input_num);
    Song *song = new Song(name);
    pm->all_songs->songs.push_back(song);
    Patch *patch = new Patch(name);
    song->patches.push_back(patch);
    for (auto& output : pm->outputs) {
      Connection *conn = new Connection(input, CONNECTION_ALL_CHANNELS,
                                        output, CONNECTION_ALL_CHANNELS);
      patch->connections.push_back(conn);
    }

    ++input_num;
  }
}
