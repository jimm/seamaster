#include <stdio.h>
#include <stdlib.h>
#include "cursor.h"
#include "debug.h"

#define UNDEFINED -1

/*
 * A PM::Cursor knows the current PM::SongList, PM::Song, and PM::Patch, how
 * to move between songs and patches, and how to find them given name
 * regexes.
 */

cursor *cursor_new(patchmaster *pm) {
  cursor *c = (cursor *)malloc(sizeof(cursor));
  c->pm = pm;
  cursor_clear(c);
  c->song_list_name = c->song_name = c->patch_name = 0;
  return c;
}

void cursor_free(cursor *c) {
  free(c);
}

void cursor_clear(cursor *c) {
  c->song_list_index = c->song_index = c->patch_index = UNDEFINED;
  // Do not erase names saved by cursor_mark();
}

/*
 * Set @song_list to All Songs, @song to first song, and
 * @patch to song's first patch. Song and patch may be +nil+.
 */
void cursor_init(cursor *c) {
  c->song_list_index = 0;

  song_list *sl = cursor_song_list(c);
  if (sl != 0 && list_length(sl->songs) > 0) {
    c->song_index = 0;
    song *s = cursor_song(c);
    c->patch_index = (s != 0 && list_length(s->patches) > 0) ? 0 : UNDEFINED;
  }
  else {
    c->song_index = UNDEFINED;
    c->patch_index = UNDEFINED;
  }
}

song_list *cursor_song_list(cursor *c) {
  if (c->song_list_index != UNDEFINED)
    return (song_list *)list_at(c->pm->song_lists, c->song_list_index);
  else
    return 0;
}

song *cursor_song(cursor *c) {
  song_list *sl = cursor_song_list(c);
  if (sl == 0 || c->song_index == UNDEFINED)
    return 0;
  return (song *)list_at(sl->songs, c->song_index);
}

patch *cursor_patch(cursor *c) {
  song *song = cursor_song(c);
  if (song == 0 || c->patch_index == UNDEFINED)
    return 0;
  return (patch *)list_at(song->patches, c->patch_index);
}

void cursor_next_song(cursor *c) {
  if (c->song_list_index == UNDEFINED)
    return;
  song_list *sl = cursor_song_list(c);
  if (c->song_index == list_length(sl->songs)-1)
    return;

  ++c->song_index;
  c->patch_index = 0;
}

void cursor_prev_song(cursor *c) {
  if (c->song_list_index == UNDEFINED || c->song_index == 0)
    return;

  --c->song_index;
  c->patch_index = 0;
}

void cursor_next_patch(cursor *c) {
  song *s = cursor_song(c);
  if (s == 0)
    return;

  if (c->patch_index == list_length(s->patches)-1)
    cursor_next_song(c);
  else
    ++c->patch_index;
}

void cursor_prev_patch(cursor *c) {
  if (c->patch_index == 0)
    cursor_prev_song(c);
  else
    --c->patch_index;
}

void cursor_debug(cursor *cursor) {
  if (cursor == 0) {
    vdebug("cursor NULL\n");
    return;
  }

  vdebug("cursor %p\n", cursor);
  vdebug("  pm %p\n", cursor->pm);
  vdebug("  song_list_index %d\n", cursor->song_list_index);
  vdebug("  song_index %d\n", cursor->song_index);
  vdebug("  patch_index %d\n", cursor->patch_index);
}
