#include <stdio.h>
#include <stdlib.h>
#include "cursor.h"

cursor *cursor_new(patchmaster *pm) {
  cursor *c = malloc(sizeof(cursor));
  c->pm = pm;
  cursor_init(c);
  return c;
}

void cursor_free(cursor *c) {
  free(c);
}

void cursor_init(cursor *c) {
  c->song_list_index = 0;

  song_list *sl = cursor_song_list(c);
  if (sl != 0 && list_length(sl->songs) > 0) {
    c->song_index = 0;
    song *s = cursor_song(c);
    c->patch_index = (s != 0 && list_length(s->patches) > 0) ? 0 : -1;
  }
  else {
    c->song_index = -1;
    c->patch_index = -1;
  }
}

song_list *cursor_song_list(cursor *c) {
  if (c->song_list_index >= 0)
    return list_at(c->pm->song_lists, c->song_list_index);
  else
    return 0;
}

song *cursor_song(cursor *c) {
  song_list *sl = cursor_song_list(c);
  if (sl != 0 && c->song_index >= 0)
    return list_at(sl->songs, c->song_index);
  else
    return 0;
}

patch *cursor_patch(cursor *c) {
  song *song = cursor_song(c);
  if (song && c->patch_index >= 0)
    return list_at(song->patches, c->patch_index);
  else
    return 0;
}

#ifdef DEBUG

void cursor_debug(cursor *cursor) {
  fprintf(stderr, "cursor %p\n", cursor);
  fprintf(stderr, "  pm %p\n", cursor->pm);
  fprintf(stderr, "  song_list_index %d\n", cursor->song_list_index);
  fprintf(stderr, "  song_index %d\n", cursor->song_index);
  fprintf(stderr, "  patch_index %d\n", cursor->patch_index);
}

#endif
