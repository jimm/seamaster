#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include "cursor.h"
#include "debug.h"

#define UNDEFINED -1

/*
 * A Cursor knows the current SongList, Song, and Patch, how to move between
 * songs and patches, and how to find them given name regexes.
 */

Cursor::Cursor(PatchMaster *pmaster)
  : pm(pmaster)
{
  clear();
}

Cursor::~Cursor() {
}

void Cursor::clear() {
  song_list_index = song_index = patch_index = UNDEFINED;
  // Do not erase names saved by mark();
}

/*
 * Set @song_list to All Songs, @song to first song, and
 * @patch to song's first patch. Song and patch may be +nil+.
 */
void Cursor::init() {
  song_list_index = 0;

  SongList *sl = song_list();
  if (sl != 0 && sl->songs.length() > 0) {
    song_index = 0;
    Song *s = song();
    patch_index = (s != 0 && s->patches.length() > 0) ? 0 : UNDEFINED;
  }
  else {
    song_index = UNDEFINED;
    patch_index = UNDEFINED;
  }
}

SongList *Cursor::song_list() {
  if (song_list_index != UNDEFINED)
    return pm->song_lists[song_list_index];
  else
    return 0;
}

Song *Cursor::song() {
  SongList *sl = song_list();
  if (sl == 0 || song_index == UNDEFINED)
    return 0;
  return sl->songs[song_index];
}

Patch *Cursor::patch() {
  Song *s = song();
  if (s == 0 || patch_index == UNDEFINED)
    return 0;
  return s->patches[patch_index];
}

void Cursor::next_song() {
  if (song_list_index == UNDEFINED)
    return;
  SongList *sl = song_list();
  if (song_index == sl->songs.length()-1)
    return;

  ++song_index;
  patch_index = 0;
}

void Cursor::prev_song() {
  if (song_list_index == UNDEFINED || song_index == 0)
    return;

  --song_index;
  patch_index = 0;
}

void Cursor::next_patch() {
  Song *s = song();
  if (s == 0)
    return;

  if (patch_index == s->patches.length()-1)
    next_song();
  else
    ++patch_index;
}

void Cursor::prev_patch() {
  if (patch_index == 0)
    prev_song();
  else
    --patch_index;
}

void Cursor::goto_song(string name_regex) {
  SongList *sl = song_list();
  Song *new_song = 0;
  Named *named;

  if (sl != 0) {
    named = find_in_list(reinterpret_cast<List<Named *> *>(&sl->songs), name_regex);
    new_song = reinterpret_cast<Song *>(named);
  }
  if (new_song == 0) {
    named = find_in_list(reinterpret_cast<List<Named *> *>(&pm->all_songs->songs),
                         name_regex);
    new_song = reinterpret_cast<Song *>(named);
  }

  if (new_song == 0)
    return;

  Patch *new_patch = new_song == 0 ? 0 : new_song->patches.first();

  SongList *new_song_list = 0;
  Song *curr_song = song();
  if ((new_song != 0 && new_song != curr_song) || (new_song == curr_song && patch() != new_patch)) {
    if (sl != 0 && sl->songs.includes(new_song))
      new_song_list = sl;
    else
      new_song_list = pm->all_songs;
  }

  song_list_index = pm->song_lists.index_of(new_song_list);
  song_index = new_song_list->songs.index_of(new_song);
  patch_index = new_song->patches.index_of(new_patch);
}

void Cursor::goto_song_list(string name_regex) {
  Named *named = find_in_list(reinterpret_cast<List<Named *> *>(&pm->song_lists), name_regex);
  if (named == 0)
    return;

  SongList *new_song_list = reinterpret_cast<SongList *>(named);
  if (new_song_list != song_list()) {
    song_list_index = pm->song_lists.index_of(new_song_list);
    song_index = 0;
    patch_index = 0;
  }
}

// Attempt to go to the same song list, song, and patch that old_cursor
// points to. Called when (re)loading a file.
void Cursor::attempt_goto(Cursor *c) {
  init();

  if (c->song_list() != 0)
    song_list_index =
      find_nearest_match_index(reinterpret_cast<List<Named *> *>(&pm->song_lists),
                               c->song_list()->name);

  if (c->song() == 0)
    return;

  song_index =
    find_nearest_match_index(reinterpret_cast<List<Named *> *>(&pm->all_songs->songs),
                             c->song()->name);
  if (c->patch() != 0)
    patch_index =
      find_nearest_match_index(reinterpret_cast<List<Named *> *>(&song()->patches),
                               c->patch()->name);
  else
    patch_index = 0;
}


Named *Cursor::find_in_list(List<Named *> *list, string regex) {
  regex_t re;
  regmatch_t pm;

  if (regcomp(&re, regex.c_str(), REG_EXTENDED | REG_ICASE) != 0)
    return 0;

  for (int i = 0; i < list->length(); ++i) {
    if (regexec(&re, list->at(i)->name.c_str(), 1, &pm, 0) == 0) {
      return list->at(i);
    }
  }
  return 0;
}

int Cursor::find_nearest_match_index(List<Named *> *list, string str) {
  string target = downcased_copy(str);
  List<int> *distances = new List<int>();
  for (int i = 0; i < list->length(); ++i)
    distances->append(damerau_levenshtein(target, downcased_copy(list->at(i)->name)));

  int min_dist = 9999, min_index = -1;
  
  for (int i = 0; i < list->length(); ++i) {
    if (distances->at(i) < min_dist) {
      min_dist = distances->at(i);
      min_index = i;
    }
  }

  delete(distances);
  return min_index;
}

int Cursor::damerau_levenshtein(string str1, string str2) {
  List<int> *oneago = 0;
  List<int> *thisrow = new List<int>();
  for (int i = 0; i < str2.length(); ++i)
    thisrow->append(i+1);
  thisrow->append(0);

  for (int x = 0; x < str1.length(); ++x) {
    List<int> *twoago = oneago;
    oneago = thisrow;
    thisrow = new List<int>();
    for (int i = 0; i < str1.length(); ++i)
      thisrow->append(0);
    thisrow->append(x+1);

    for (int y = 0; y < str2.length(); ++y) {
      int delcost = oneago->at(y) + 1;
      int addcost = thisrow->at(y-1) + 1;
      int subcost = oneago->at(y-1) + ((str1[x] != str2[y]) ? 1 : 0);

      int minval = delcost < addcost ? delcost : addcost;
      minval = subcost < minval ? subcost : minval;
      thisrow->at_set(y, minval);

      if (x > 0 && y > 0 && str1[x] == str2[y-1] && str1[x-1] == str2[y] && str1[x] != str2[y]) {
        minval = thisrow->at(y) < (twoago->at(y-2) + 1) ? thisrow->at(y) : (twoago->at(y-2) + 1);
        thisrow->at_set(y, minval);
      }
    }
  }

  if (oneago) delete(oneago);
  delete(thisrow);
  return thisrow->at(str2.length() - 1);
}

string Cursor::downcased_copy(string str) {
  string downcased = string(str);
  for (string::iterator i = downcased.begin(); i != downcased.end(); ++i)
    *i = tolower(*i);
  return downcased;
}

void Cursor::debug() {
  vdebug("cursor %p\n", this);
  vdebug("  pm %p\n", pm);
  vdebug("  song_list_index %d\n", song_list_index);
  vdebug("  song_index %d\n", song_index);
  vdebug("  patch_index %d\n", patch_index);
}
