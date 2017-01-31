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
  list *inputs;
  list *outputs;
  SongList *all_songs;
  list *song_lists;
  Cursor *cursor;
  bool running;
  bool testing;
  list *messages;
  list *triggers;

  PatchMaster();
  ~PatchMaster();

  int load(const char *);

  void start();
  void stop();

  void next_patch();
  void prev_patch();
  void next_song();
  void prev_song();

  void debug();
};

#endif /* PATCHMASTER_H */
