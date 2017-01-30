#ifndef SONG_H
#define SONG_H

#include "list.h"
#include "patch.h"

class Song : public Named {
public:
  list *notes;
  list *patches;

  Song(const char *name);
  ~Song();

  void append_notes(char *);

  void debug();
};

#endif /* SONG_H */
