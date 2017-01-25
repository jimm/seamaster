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
  cursor *c = malloc(sizeof(cursor));
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
    return list_at(c->pm->song_lists, c->song_list_index);
  else
    return 0;
}

song *cursor_song(cursor *c) {
  song_list *sl = cursor_song_list(c);
  if (sl == 0 || c->song_index == UNDEFINED)
    return 0;
  return list_at(sl->songs, c->song_index);
}

patch *cursor_patch(cursor *c) {
  song *song = cursor_song(c);
  if (song == 0 || c->patch_index == UNDEFINED)
    return 0;
  return list_at(song->patches, c->patch_index);
}

void cursor_next_song(cursor *c) {
  if (c->song_list_index == UNDEFINED)
    return;
  song_list *sl = cursor_song_list(c);
  if (list_last(sl->songs) == cursor_song(c))
    return;

  patch_start(cursor_patch(c));
  ++c->song_index;
  c->patch_index = 0;
  patch_start(cursor_patch(c));
}

void cursor_prev_song(cursor *c) {
  if (c->song_list_index == UNDEFINED)
    return;
  song_list *sl = cursor_song_list(c);
  if (list_at(sl->songs, 0) == cursor_song(c))
    return;

  patch_start(cursor_patch(c));
  --c->song_index;
  c->patch_index = 0;
  patch_start(cursor_patch(c));
}

void cursor_next_patch(cursor *c) {
  song *s = cursor_song(c);
  if (s == 0)
    return;

  patch *p = cursor_patch(c);
  if (list_last(s->patches) == p)
    cursor_next_song(c);
  else if (p != 0) {
    patch_start(p);
    --c->patch_index;
    patch_start(cursor_patch(c));
  }
}

void cursor_prev_patch(cursor *c) {
  song *s = cursor_song(c);
  if (s == 0)
    return;

  patch *p = cursor_patch(c);
  if (list_at(s->patches, 0) == p)
    cursor_prev_song(c);
  else if (p != 0) {
    patch_start(p);
    ++c->patch_index;
    patch_start(cursor_patch(c));
  }
}

void cursor_debug(cursor *cursor) {
  debug("cursor %p\n", cursor);
  debug("  pm %p\n", cursor->pm);
  debug("  song_list_index %d\n", cursor->song_list_index);
  debug("  song_index %d\n", cursor->song_index);
  debug("  patch_index %d\n", cursor->patch_index);
}
