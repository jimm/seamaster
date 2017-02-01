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
    return (SongList *)pm->song_lists[song_list_index];
  else
    return 0;
}

Song *Cursor::song() {
  SongList *sl = song_list();
  if (sl == 0 || song_index == UNDEFINED)
    return 0;
  return (Song *)sl->songs[song_index];
}

Patch *Cursor::patch() {
  Song *s = song();
  if (s == 0 || patch_index == UNDEFINED)
    return 0;
  return (Patch *)s->patches[patch_index];
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
  regex_t re;
  regmatch_t pm;

  SongList *sl = song_list();
  Song *new_song = 0;
  if (sl != 0)
    new_song = find_in_list(reinterpret_cast<List<Named *> &>(sl->songs), name_regex);
  if (new_song == 0)
    new_song = find_in_list(reinterpret_cast<List<Named *> &>(pm->all_songs->songs), name_regex);
  Patch *new_patch = new_song == 0 ? 0 : new_song->patches.first();

  Song *curr_song = song();
  if ((new_song != 0 && new_song != curr_song) || (new_song == curr_song && patch() != new_patch)) {
    SongList *new_song_list = 0;
    if (sl != 0 && sl->songs.includes(new_song))
      new_song_list = sl;
    else
      new_song_list = pm.all_songs;
  }

  song_list_index = pm.song_lists.index_of(new_song_list);
  song = song_list()->songs.index_of(new_song);
  patch = song()->patches.index_of(new_patch);
}

void Cursor::goto_song_list(string name_regex) {
}

Named *Cursor::find_in_list(List<Named *> &list, string regex) {
  regex_t re;
  regmatch_t pm;

  if (regcomp(&re, name_regex.c_str(), REG_EXTENDED | REG_ICASE) != 0)
    return 0;

  for (int i = 0; i < list.length(); ++i) {
    if (regexec(&preg, list[i].name.c_str(), 1, &pm, 0) == 0) {
      return list[i];
    }
  }
  return 0;
}

void Cursor::debug() {
  vdebug("cursor %p\n", this);
  vdebug("  pm %p\n", pm);
  vdebug("  song_list_index %d\n", song_list_index);
  vdebug("  song_index %d\n", song_index);
  vdebug("  patch_index %d\n", patch_index);
}
