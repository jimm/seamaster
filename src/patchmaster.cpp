#include <stdlib.h>
#include "patchmaster.h"
#include "cursor.h"
#include "debug.h"

// ================ allocation ================

patchmaster *patchmaster_new() {
  patchmaster *pm = (patchmaster *)malloc(sizeof(patchmaster));
  pm->running = false;
  pm->testing = false;
  pm->inputs = list_new();
  pm->outputs = list_new();
  pm->all_songs = song_list_new((char *)"All Songs"); /* TODO sorted song list */
  pm->song_lists = list_new();
  list_append(pm->song_lists, pm->all_songs);
  pm->messages = list_new();
  pm->triggers = list_new();
  pm->cursor = cursor_new(pm);
  patchmaster_debug(pm);
  return pm;
}

void patchmaster_free(patchmaster *pm) {
  for (int i = 0; i < list_length(pm->inputs); ++i)
    input_free((input *)list_at(pm->inputs, i));
  list_free(pm->inputs, 0);
  for (int i = 0; i < list_length(pm->outputs); ++i)
    output_free((output *)list_at(pm->outputs, i));
  list_free(pm->outputs, 0);
  for (int i = 0; i < list_length(pm->all_songs->songs); ++i)
    delete (Song *)list_at(pm->all_songs->songs, i);
  for (int i = 0; i < list_length(pm->song_lists); ++i)
    song_list_free((song_list *)list_at(pm->song_lists, i));
  list_free(pm->song_lists, 0);
  for (int i = 0; i < list_length(pm->messages); ++i)
    message_free((message *)list_at(pm->messages, i));
  list_free(pm->messages, 0);
  for (int i = 0; i < list_length(pm->triggers); ++i)
    trigger_free((trigger *)list_at(pm->triggers, i));
  list_free(pm->triggers, 0);
  free(pm);
}

// ================ running ================

void patchmaster_start(patchmaster *pm) {
  vdebug("patchmaster_start\n");
  for (int i = 0; i < list_length(pm->inputs); ++i)
    input_start((input *)list_at(pm->inputs, i));
  cursor_init(pm->cursor);
  if (cursor_patch(pm->cursor) != 0)
    cursor_patch(pm->cursor)->start();
  pm->running = true;
}

void patchmaster_stop(patchmaster *pm) {
  vdebug("patchmaster_stop\n");
  if (cursor_patch(pm->cursor) != 0)
    cursor_patch(pm->cursor)->stop();
  for (int i = 0; i < list_length(pm->inputs); ++i)
    input_stop((input *)list_at(pm->inputs, i));
  pm->running = false;
}

// ================ movement ================

void patchmaster_next_patch(patchmaster *pm) {
  if (cursor_patch(pm->cursor) != 0)
    cursor_patch(pm->cursor)->stop();
  cursor_next_patch(pm->cursor);
  if (cursor_patch(pm->cursor) != 0)
    cursor_patch(pm->cursor)->start();
}

void patchmaster_prev_patch(patchmaster *pm) {
  if (cursor_patch(pm->cursor) != 0)
    cursor_patch(pm->cursor)->stop();
  cursor_prev_patch(pm->cursor);
  if (cursor_patch(pm->cursor) != 0)
    cursor_patch(pm->cursor)->start();
}

void patchmaster_next_song(patchmaster *pm) {
  if (cursor_patch(pm->cursor) != 0)
    cursor_patch(pm->cursor)->stop();
  cursor_next_song(pm->cursor);
  if (cursor_patch(pm->cursor) != 0)
    cursor_patch(pm->cursor)->start();
}

void patchmaster_prev_song(patchmaster *pm) {
  if (cursor_patch(pm->cursor) != 0)
    cursor_patch(pm->cursor)->stop();
  cursor_prev_song(pm->cursor);
  if (cursor_patch(pm->cursor) != 0)
    cursor_patch(pm->cursor)->start();
}

// ================ vdebugging ================

void patchmaster_debug(patchmaster *pm) {
  if (pm == 0) {
    vdebug("patchmaster NULL\n");
    return;
  }

  vdebug("patchmaster %p, running %d\n", pm, pm->running);
  for (int i = 0; i < list_length(pm->inputs); ++i)
    input_debug((input *)list_at(pm->inputs, i));
  for (int i = 0; i < list_length(pm->outputs); ++i)
    output_debug((output *)list_at(pm->outputs, i));
  for (int i = 0; i < list_length(pm->all_songs->songs); ++i)
    ((Song *)list_at(pm->all_songs->songs, i))->debug();
  list_debug(pm->song_lists, "pm->song_lists");
  list_debug(pm->messages, "pm->messages");
  list_debug(pm->triggers, "pm->triggers");
  cursor_debug(pm->cursor);
}
