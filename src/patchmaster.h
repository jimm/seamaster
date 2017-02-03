#ifndef PATCHMASTER_H
#define PATCHMASTER_H

#include "song_list.h"
#include "list.h"
#include "input.h"
#include "output.h"
#include "message.h"

class Cursor;

class PatchMaster {
public:
  List<Input *> inputs;
  List<Output *> outputs;
  SongList *all_songs;
  List<SongList *> song_lists;
  Cursor *cursor;
  bool running;
  bool testing;
  List<Message *> messages;
  List<Trigger *> triggers;

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

  void debug();
};

#endif /* PATCHMASTER_H */
