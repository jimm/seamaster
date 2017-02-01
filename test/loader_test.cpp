#include <string.h>
#include "test_helper.h"
#include "../src/loader.h"
#include "loader_test.h"

#define TEST_FILE "test/testfile.sm"

PatchMaster *load_test_file() {
  PatchMaster *pm = create_pm();
  Loader loader(*pm);
  loader.load(TEST_FILE);
  return pm;
}

void test_load_instruments() {
  PatchMaster *pm = load_test_file();
  tassert(pm->inputs.length() == 2, "bad num inputs");
  tassert(pm->outputs.length() == 2, "bad num outputs");

  Input *in = (Input *)pm->inputs.first();
  tassert(in->sym == "one", "bad input sym");
  tassert(in->name == "first input", "bad input name");

  Output *out = (Output *)pm->outputs.last();
  tassert(out->sym == "two", "bad output sym");
  tassert(out->name == "second output", "bad output name");
  delete pm;
}

void test_load_messages() {
  PatchMaster *pm = load_test_file();
  tassert(pm->messages.length() == 0, "no message loading yet!");
  delete pm;
}

void test_load_triggers() {
  PatchMaster *pm = load_test_file();
  tassert(pm->triggers.length() == 0, "no trigger loading yet!");
  delete pm;
}

void test_load_songs() {
  PatchMaster *pm = load_test_file();
  List &all = pm->all_songs->songs;
  tassert(all.length() == 2, "wrong num songs loaded");

  Song *s = (Song *)all.first();
  tassert(s->name == "To Each His Own", "bad song title");

  s = (Song *)all.last();
  tassert(s->name == "Another Song", "bad song title");
  delete pm;
}

void test_load_notes() {
  PatchMaster *pm = load_test_file();
  Song *s = (Song *)pm->all_songs->songs.first();
  tassert(s->notes.length() == 0, "extra notes?");

  s = (Song *)pm->all_songs->songs.last();
  tassert(s->notes.length() == 2, "bad notes length");
  tassert(strcmp((const char *)s->notes.first(), "this song has note text\n") == 0,
          "bad notes 1");
  tassert(strcmp((const char *)s->notes.last(), "that spans multiple lines\n") == 0,
          "bad notes 2");
  delete pm;
}

void test_load_patches() {
  PatchMaster *pm = load_test_file();
  Song *s = (Song *)pm->all_songs->songs.first();
  tassert(s->patches.length() == 2, "bad num patches");

  Patch *p = (Patch *)s->patches.first();
  tassert(p->name == "Vanilla Through, Filter Two's Sustain",
          "bad patch name");
  delete pm;
}

void test_load_connections() {
  PatchMaster *pm = load_test_file();
  Song *s = (Song *)pm->all_songs->songs.first();
  Patch *p = (Patch *)s->patches.first();
  tassert(p->connections.length() == 2, "bad num conns");
  Connection *conn = (Connection *)p->connections.first();
  tassert(conn->input == pm->inputs.first(), "wrong conn input");
  tassert(conn->input_chan == -1, "wrong conn input channel");
  tassert(conn->output == pm->outputs.first(), "wrong conn output");
  tassert(conn->output_chan == -1, "wrong conn output channel");

  s = (Song *)pm->all_songs->songs.last();
  p = (Patch *)s->patches.last();
  tassert(p->connections.length() == 2, "bad num conns");
  conn = (Connection *)p->connections.first();
  tassert(conn->input_chan == 2, "wrong conn input channel");
  tassert(conn->output_chan == 3, "wrong conn output channel");
  delete pm;
}

void test_load_bank() {
  PatchMaster *pm = load_test_file();
  Song *s = (Song *)pm->all_songs->songs.first();
  Patch *p = (Patch *)s->patches.first();
  Connection *conn = (Connection *)p->connections.last();
  tassert(conn->prog.bank_msb = 3, "bad bank msb");
  tassert(conn->prog.bank_lsb = 2, "bad bank lsb");
}

void test_load_prog_chg() {
  PatchMaster *pm = load_test_file();
  Song *s = (Song *)pm->all_songs->songs.first();
  Patch *p = (Patch *)s->patches.first();
  Connection *conn = (Connection *)p->connections.last();
  tassert(conn->prog.prog = 12, "bad program change");
}

void test_load_xpose() {
  PatchMaster *pm = load_test_file();
  Song *s = (Song *)pm->all_songs->songs.first();
  Patch *p = (Patch *)s->patches.first();
  Connection *conn = (Connection *)p->connections.first();
  tassert(conn->xpose == 0, "bad default xpose");

  p = (Patch *)s->patches.last();
  conn = (Connection *)p->connections.first();
  tassert(conn->xpose = 12, "bad xpose");
  conn = (Connection *)p->connections.last();
  tassert(conn->xpose = -12, "bad xpose");
  delete pm;
}

void test_load_filter() {
  PatchMaster *pm = load_test_file();
  Song *s = (Song *)pm->all_songs->songs.first();
  Patch *p = (Patch *)s->patches.first();
  Connection *conn = (Connection *)p->connections.last();
  tassert(conn->cc_maps[64] == -1, "bad cc filter");
  delete pm;
}

void test_load_map() {
  PatchMaster *pm = load_test_file();
  Song *s = (Song *)pm->all_songs->songs.first();
  Patch *p = (Patch *)s->patches.first();
  Connection *conn = (Connection *)p->connections.first();
  tassert(conn->cc_maps[7] == 7, "bad default map");

  p = (Patch *)s->patches.last();
  conn = (Connection *)p->connections.first();
  tassert(conn->cc_maps[7] == 10, "bad cc remapping");
  delete pm;
}

void test_load_song_list() {
  PatchMaster *pm = load_test_file();
  List &all = pm->all_songs->songs;

  tassert(pm->song_lists.length() == 3, "bad num song lists");
  SongList *sl = (SongList *)pm->song_lists.last();
  tassert(sl->name == "Song List Two", "bad song list name");

  tassert(sl->songs.length() == 2, "bad num songs in song list");
  tassert(sl->songs.first() == all.last(), "wrong song");
  tassert(sl->songs.last() == all.first(), "wrong song");
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
  test_run(test_load_bank);
  test_run(test_load_prog_chg);
  test_run(test_load_xpose);
  test_run(test_load_filter);
  test_run(test_load_map);
  test_run(test_load_song_list);
}
