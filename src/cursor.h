#ifndef CURSOR_H
#define CURSOR_H

#include "patchmaster.h"
#include "song_list.h"
#include "song.h"
#include "patch.h"

class Cursor {
public:
  PatchMaster *pm;
  int song_list_index;
  int song_index;
  int patch_index;

  Cursor(PatchMaster *pm);
  ~Cursor();

  void clear();
  void init();

  SongList *song_list();
  Song *song();
  Patch *patch();

  void next_song();
  void prev_song();
  void next_patch();
  void prev_patch();

  void goto_song(string name_regex);
  void goto_song_list(string name_regex);
  void attempt_goto(Cursor *old_cursor);

private:
  Named *find_in_list(vector<Named *> *, string regex);
  int find_nearest_match_index(vector<Named *> *, string str);
  unsigned int damerau_levenshtein(const std::string& s1, const std::string& s2);
  string downcased_copy(string str);
};

#endif /* CURSOR_H */
