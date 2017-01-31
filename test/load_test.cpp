#include <string.h>
#include "test_helper.h"
#include "../src/load.h"
#include "load_test.h"

#define TEST_FILE "test/testfile.sm"

PatchMaster *load_test_file() {
  PatchMaster *pm = create_pm();
  load(pm, TEST_FILE);
  return pm;
}

void test_load_instruments() {
  PatchMaster *pm = load_test_file();
  tassert(list_length(pm->inputs) == 2, "bad num inputs");
  tassert(list_length(pm->outputs) == 2, "bad num outputs");

  Input *in = (Input *)list_first(pm->inputs);
  tassert(in->sym == "one", "bad input sym");
  tassert(in->name == "first input", "bad input name");

  Output *out = (Output *)list_last(pm->outputs);
  tassert(out->sym == "two", "bad output sym");
  tassert(out->name == "second output", "bad output name");
  delete pm;
}

void test_load_messages() {
  PatchMaster *pm = load_test_file();
  tassert(list_length(pm->messages) == 0, "no message loading yet!");
  delete pm;
}

void test_load_triggers() {
  PatchMaster *pm = load_test_file();
  tassert(list_length(pm->triggers) == 0, "no trigger loading yet!");
  delete pm;
}

void test_load_songs() {
  PatchMaster *pm = load_test_file();
  list *all = pm->all_songs->songs;
  tassert(list_length(all) == 2, "wrong num songs loaded");

  Song *s = (Song *)list_first(all);
  tassert(s->name == "To Each His Own", "bad song title");

  s = (Song *)list_last(all);
  tassert(s->name == "Another Song", "bad song title");
  delete pm;
}

void test_load_notes() {
  PatchMaster *pm = load_test_file();
  Song *s = (Song *)list_first(pm->all_songs->songs);
  tassert(list_length(s->notes) == 0, "extra notes?");

  s = (Song *)list_last(pm->all_songs->songs);
  tassert(list_length(s->notes) == 2, "bad notes length");
  tassert(strcmp((const char *)list_first(s->notes), "this song has note text\n") == 0,
          "bad notes 1");
  tassert(strcmp((const char *)list_last(s->notes), "that spans multiple lines\n") == 0,
          "bad notes 2");
  delete pm;
}

void test_load_patches() {
  PatchMaster *pm = load_test_file();
  Song *s = (Song *)list_first(pm->all_songs->songs);
  tassert(list_length(s->patches) == 2, "bad num patches");

  Patch *p = (Patch *)list_first(s->patches);
  tassert(p->name == "Vanilla Through, Filter Two's Sustain",
          "bad patch name");
  delete pm;
}

void test_load_connections() {
  PatchMaster *pm = load_test_file();
  Song *s = (Song *)list_first(pm->all_songs->songs);
  Patch *p = (Patch *)list_first(s->patches);
  tassert(list_length(p->connections) == 2, "bad num conns");
  Connection *conn = (Connection *)list_first(p->connections);
  tassert(conn->input == list_first(pm->inputs), "wrong conn input");
  tassert(conn->input_chan == -1, "wrong conn input channel");
  tassert(conn->output == list_first(pm->outputs), "wrong conn output");
  tassert(conn->output_chan == -1, "wrong conn output channel");

  s = (Song *)list_last(pm->all_songs->songs);
  p = (Patch *)list_last(s->patches);
  tassert(list_length(p->connections) == 2, "bad num conns");
  conn = (Connection *)list_first(p->connections);
  tassert(conn->input_chan == 2, "wrong conn input channel");
  tassert(conn->output_chan == 3, "wrong conn output channel");
  delete pm;
}

void test_load_xpose() {
  PatchMaster *pm = load_test_file();
  Song *s = (Song *)list_first(pm->all_songs->songs);
  Patch *p = (Patch *)list_first(s->patches);
  Connection *conn = (Connection *)list_first(p->connections);
  tassert(conn->xpose == 0, "bad default xpose");

  p = (Patch *)list_last(s->patches);
  conn = (Connection *)list_first(p->connections);
  tassert(conn->xpose = 12, "bad xpose");
  conn = (Connection *)list_last(p->connections);
  tassert(conn->xpose = -12, "bad xpose");
  delete pm;
}

void test_load_filter() {
  PatchMaster *pm = load_test_file();
  Song *s = (Song *)list_first(pm->all_songs->songs);
  Patch *p = (Patch *)list_first(s->patches);
  Connection *conn = (Connection *)list_last(p->connections);
  tassert(conn->cc_maps[64] == -1, "bad cc filter");
  delete pm;
}

void test_load_map() {
  PatchMaster *pm = load_test_file();
  Song *s = (Song *)list_first(pm->all_songs->songs);
  Patch *p = (Patch *)list_first(s->patches);
  Connection *conn = (Connection *)list_first(p->connections);
  tassert(conn->cc_maps[7] == 7, "bad default map");

  p = (Patch *)list_last(s->patches);
  conn = (Connection *)list_first(p->connections);
  tassert(conn->cc_maps[7] == 10, "bad cc remapping");
  delete pm;
}

void test_load_song_list() {
  PatchMaster *pm = load_test_file();
  list *all = pm->all_songs->songs;

  tassert(list_length(pm->song_lists) == 3, "bad num song lists");
  SongList *sl = (SongList *)list_last(pm->song_lists);
  tassert(sl->name == "Song List Two", "bad song list name");

  tassert(list_length(sl->songs) == 2, "bad num songs in song list");
  tassert(list_first(sl->songs) == list_last(all), "wrong song");
  tassert(list_last(sl->songs) == list_first(all), "wrong song");
  delete pm;
}

void test_load() {
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
