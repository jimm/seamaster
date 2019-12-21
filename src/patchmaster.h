#ifndef PATCHMASTER_H
#define PATCHMASTER_H

#include <vector>
#include "song_list.h"
#include "input.h"
#include "output.h"
#include "message.h"

using namespace std;

class Cursor;

class PatchMaster {
public:
  vector<Input *> inputs;
  vector<Output *> outputs;
  SongList *all_songs;
  vector<SongList *> song_lists;
  Cursor *cursor;
  bool running;
  bool testing;
  vector<Message *> messages;
  string loaded_from_file;

  PatchMaster();
  ~PatchMaster();

  int load(const char *);

  void start();
  void stop();

  void next_patch();
  void prev_patch();
  void next_song();
  void prev_song();

  void goto_song(string name_regex);
  void goto_song_list(string name_regex);

  void panic(bool send_notes_off);
};

PatchMaster *PatchMaster_instance();

#endif /* PATCHMASTER_H */
