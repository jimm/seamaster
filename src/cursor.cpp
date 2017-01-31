#include <stdio.h>
#include <stdlib.h>
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

void Cursor::debug() {
  vdebug("cursor %p\n", this);
  vdebug("  pm %p\n", pm);
  vdebug("  song_list_index %d\n", song_list_index);
  vdebug("  song_index %d\n", song_index);
  vdebug("  patch_index %d\n", patch_index);
}
