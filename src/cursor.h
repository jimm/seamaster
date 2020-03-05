#ifndef CURSOR_H
#define CURSOR_H

#include "patchmaster.h"
#include "set_list.h"
#include "song.h"
#include "patch.h"

class Cursor {
public:
  PatchMaster *pm;
  int set_list_index;
  int song_index;
  int patch_index;

  Cursor(PatchMaster *pm);
  ~Cursor();

  void clear();
  void init();

  SetList *set_list();
  Song *song();
  Patch *patch();

  void next_song();
  void prev_song();
  void next_patch();
  void prev_patch();

  bool has_next_song();
  bool has_prev_song();
  bool has_next_patch();
  bool has_prev_patch();

  void jump_to_set_list_index(int i);
  void jump_to_song_index(int i);
  void jump_to_patch_index(int i);

  void goto_song(string name_regex);
  void goto_set_list(string name_regex);
  void attempt_goto(Cursor *old_cursor);

private:
  Named *find_in_list(vector<Named *> *, string regex);
  int find_nearest_match_index(vector<Named *> *, string str);
  unsigned int damerau_levenshtein(const std::string& s1, const std::string& s2);
  string downcased_copy(string str);
};

#endif /* CURSOR_H */
