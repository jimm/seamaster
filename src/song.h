#ifndef SONG_H
#define SONG_H

#include "list.h"
#include "patch.h"

class Song : public Named {
public:
  List patches;
  List notes;

  Song(const char *name);
  ~Song();

  void append_notes(char *);

  void debug();
};

#endif /* SONG_H */
