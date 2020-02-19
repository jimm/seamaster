#ifndef PATCHMASTER_H
#define PATCHMASTER_H

#include <vector>
#include "set_list.h"
#include "input.h"
#include "output.h"
#include "message.h"

using namespace std;

class Cursor;

class PatchMaster {
public:
  vector<Input *> inputs;
  vector<Output *> outputs;
  SetList *all_songs;
  vector<SetList *> set_lists;
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
  void goto_set_list(string name_regex);

  void jump_to_set_list_index(int i);
  void jump_to_song_index(int i);
  void jump_to_patch_index(int i);

  void panic(bool send_notes_off);
};

PatchMaster *PatchMaster_instance();

#endif /* PATCHMASTER_H */
