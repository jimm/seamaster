#include <stdlib.h>
#include "patchmaster.h"
#include "cursor.h"
#include "debug.h"

// ================ allocation ================

patchmaster *patchmaster_new() {
  patchmaster *pm = malloc(sizeof(patchmaster));
  pm->running = false;
  pm->inputs = list_new();
  pm->outputs = list_new();
  pm->all_songs = song_list_new("All Songs"); /* TODO sorted song list */
  pm->song_lists = list_new();
  list_append(pm->song_lists, pm->all_songs);
  pm->messages = list_new();
  pm->triggers = list_new();
  pm->cursor = cursor_new(pm);
  patchmaster_debug(pm);
  return pm;
}

void patchmaster_free(patchmaster *pm) {
  list_free(pm->inputs, input_free);
  list_free(pm->outputs, output_free);
  for (int i = 0; i < list_length(pm->all_songs->songs); ++i)
    song_free(list_at(pm->all_songs->songs, i));
  list_free(pm->song_lists, song_list_free); /* includes pm->all_songs */
  list_free(pm->messages, message_free);
  list_free(pm->triggers, trigger_free);
  free(pm);
}

// ================ running ================

void patchmaster_start(patchmaster *pm) {
  debug("patchmaster_start\n");
  cursor_init(pm->cursor);
  for (int i = 0; i < list_length(pm->inputs); ++i)
    input_start((input *)list_at(pm->inputs, i));
  if (cursor_patch(pm->cursor))
    patch_start(cursor_patch(pm->cursor));
  pm->running = true;
}

void patchmaster_stop(patchmaster *pm) {
  debug("patchmaster_stop\n");
  if (cursor_patch(pm->cursor))
    patch_stop(cursor_patch(pm->cursor));
  for (int i = 0; i < list_length(pm->inputs); ++i)
    input_stop((input *)list_at(pm->inputs, i));
  pm->running = false;
}

// ================ movement ================

void patchmaster_next_patch(patchmaster *pm) {
  cursor_next_patch(pm->cursor);
}

void patchmaster_prev_patch(patchmaster *pm) {
  cursor_prev_patch(pm->cursor);
}

void patchmaster_next_song(patchmaster *pm) {
  cursor_next_song(pm->cursor);
}

void patchmaster_prev_song(patchmaster *pm) {
  cursor_prev_song(pm->cursor);
}

// ================ debugging ================

void patchmaster_debug(patchmaster *pm) {
  debug("patchmaster %p, running %d\n", pm, pm->running);
  for (int i = 0; i < list_length(pm->inputs); ++i)
    input_debug(list_at(pm->inputs, i));
  for (int i = 0; i < list_length(pm->outputs); ++i)
    output_debug(list_at(pm->outputs, i));
  for (int i = 0; i < list_length(pm->all_songs->songs); ++i)
    song_debug(list_at(pm->all_songs->songs, i));
  list_debug(pm->song_lists, "pm->song_lists");
  list_debug(pm->messages, "pm->messages");
  list_debug(pm->triggers, "pm->triggers");
  cursor_debug(pm->cursor);
}
