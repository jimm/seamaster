#ifndef SONG_H
#define SONG_H

#include "list.h"
#include "patch.h"

class Song : public Named {
public:
  List<Patch *> patches;
  List<char *> notes;

  Song(const char *name);
  ~Song();

  void take_notes(List<char *> &);

  void debug();
};

#endif /* SONG_H */
