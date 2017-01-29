#include <string.h>
#include "seamaster_test.h"
#include "../src/load.h"
#include "load_test.h"

void test_load_instruments(patchmaster *pm) {
  tassert(list_length(pm->inputs) == 2, "bad num inputs");
  tassert(list_length(pm->outputs) == 2, "bad num outputs");

  input *in = list_first(pm->inputs);
  tassert(strcmp(in->sym, "one") == 0, "bad input sym");
  tassert(strcmp(in->name, "first input") == 0, "bad input name");

  output *out = list_last(pm->outputs);
  tassert(strcmp(out->sym, "two") == 0, "bad output sym");
  tassert(strcmp(out->name, "second output") == 0, "bad output name");
}

void test_load_messages(patchmaster *pm) {
  tassert(list_length(pm->messages) == 0, "no message loading yet!");
}

void test_load_triggers(patchmaster *pm) {
  tassert(list_length(pm->triggers) == 0, "no trigger loading yet!");
}

void test_load_songs(patchmaster *pm) {
  list *all = pm->all_songs->songs;
  tassert(list_length(all) == 2, "wrong num songs loaded");

  song *s = list_first(all);
  tassert(strcmp(s->name, "To Each His Own") == 0, "bad song title");

  s = list_last(all);
  tassert(strcmp(s->name, "Another Song") == 0, "bad song title");
}

void test_load_notes(patchmaster *pm) {
  song *s = list_first(pm->all_songs->songs);
  tassert(list_length(s->notes) == 0, "extra notes?");

  s = list_last(pm->all_songs->songs);
  tassert(list_length(s->notes) == 2, "bad notes length");
  tassert(strcmp(list_first(s->notes), "this song has note text\n") == 0,
          "bad notes 1");
  tassert(strcmp(list_last(s->notes), "that spans multiple lines\n") == 0,
          "bad notes 2");
}

void test_load_patches(patchmaster *pm) {
  song *s = list_first(pm->all_songs->songs);
  tassert(list_length(s->patches) == 2, "bad num patches");

  patch *p = list_first(s->patches);
  tassert(strcmp(p->name, "Vanilla Through, Filter Two's Sustain") == 0,
          "bad patch name");
}

void test_load_connections(patchmaster *pm) {
  song *s = list_first(pm->all_songs->songs);
  patch *p = list_first(s->patches);
  tassert(list_length(p->connections) == 2, "bad num conns");
  connection *conn = list_first(p->connections);
  tassert(conn->input == list_first(pm->inputs), "wrong conn input");
  tassert(conn->input_chan == -1, "wrong conn input channel");
  tassert(conn->output == list_first(pm->outputs), "wrong conn output");
  tassert(conn->output_chan == -1, "wrong conn output channel");

  s = list_last(pm->all_songs->songs);
  p = list_last(s->patches);
  tassert(list_length(p->connections) == 2, "bad num conns");
  conn = list_first(p->connections);
  tassert(conn->input_chan == 2, "wrong conn input channel");
  tassert(conn->output_chan == 3, "wrong conn output channel");
}

void test_load_xpose(patchmaster *pm) {
  song *s = list_first(pm->all_songs->songs);
  patch *p = list_first(s->patches);
  connection *conn = list_first(p->connections);
  tassert(conn->xpose == 0, "bad default xpose");

  p = list_last(s->patches);
  conn = list_first(p->connections);
  tassert(conn->xpose = 12, "bad xpose");
  conn = list_last(p->connections);
  tassert(conn->xpose = -12, "bad xpose");
}

void test_load_filter(patchmaster *pm) {
  song *s = list_first(pm->all_songs->songs);
  patch *p = list_first(s->patches);
  connection *conn = list_last(p->connections);
  tassert(conn->cc_maps[64] == -1, "bad cc filter");
}

void test_load_map(patchmaster *pm) {
  song *s = list_first(pm->all_songs->songs);
  patch *p = list_first(s->patches);
  connection *conn = list_first(p->connections);
  tassert(conn->cc_maps[7] == 7, "bad default map");

  p = list_last(s->patches);
  conn = list_first(p->connections);
  tassert(conn->cc_maps[7] == 10, "bad cc remapping");
}

void test_load_song_list(patchmaster *pm) {
  list *all = pm->all_songs->songs;

  tassert(list_length(pm->song_lists) == 3, "bad num song lists");
  song_list *sl = list_last(pm->song_lists);
  tassert(strcmp(sl->name, "Song List Two") == 0, "bad song list name");

  tassert(list_length(sl->songs) == 2, "bad num songs in song list");
  tassert(list_first(sl->songs) == list_last(all), "wrong song");
  tassert(list_last(sl->songs) == list_first(all), "wrong song");
}

void test_load(patchmaster *pm) {
  test_run(test_load_instruments);
  test_run(test_load_messages);
  test_run(test_load_triggers);
  test_run(test_load_songs);
  test_run(test_load_notes);
  test_run(test_load_patches);
  test_run(test_load_connections);
  test_run(test_load_xpose);
  test_run(test_load_filter);
  test_run(test_load_map);
  test_run(test_load_song_list);
}
