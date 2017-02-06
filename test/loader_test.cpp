#include <string.h>
#include "test_helper.h"
#include "../src/loader.h"
#include "loader_test.h"

#define TEST_FILE "test/testfile.org"

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

  Input *in = pm->inputs.first();
  tassert(in->sym == "one", "bad input sym");
  tassert(in->name == "first input", "bad input name");

  Output *out = pm->outputs.last();
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
  List<Song *> &all = pm->all_songs->songs;
  tassert(all.length() == 2, "wrong num songs loaded");

  Song *s = all.first();
  tassert(s->name == "To Each His Own", "bad song title");

  s = all.last();
  tassert(s->name == "Another Song", "bad song title");
  delete pm;
}

void test_load_notes() {
  PatchMaster *pm = load_test_file();
  Song *s = pm->all_songs->songs.first();
  tassert(s->notes.length() == 0, "extra notes?");

  s = pm->all_songs->songs.last();
  tassert(s->notes.length() == 3, "bad notes length");
  tassert(strcmp((const char *)s->notes[0],
                 "the line before begin_example contains only whitespace") == 0,
          "bad notes 1");
  tassert(strcmp((const char *)s->notes[1], "this song has note text") == 0,
          "bad notes 2");
  tassert(strcmp((const char *)s->notes[2], "that spans multiple lines") == 0,
          "bad notes 3");
  delete pm;
}

void test_load_patches() {
  PatchMaster *pm = load_test_file();
  Song *s = pm->all_songs->songs.first();
  tassert(s->patches.length() == 2, "bad num patches");

  Patch *p = s->patches.first();
  tassert(p->name == "Vanilla Through, Filter Two's Sustain",
          "bad patch name");
  delete pm;
}

void test_load_connections() {
  PatchMaster *pm = load_test_file();
  Song *s = pm->all_songs->songs.first();
  Patch *p = s->patches.first();
  tassert(p->connections.length() == 2, "bad num conns");
  Connection *conn = p->connections.first();
  tassert(conn->input == pm->inputs.first(), "wrong conn input");
  tassert(conn->input_chan == -1, "wrong conn input channel");
  tassert(conn->output == pm->outputs.first(), "wrong conn output");
  tassert(conn->output_chan == -1, "wrong conn output channel");

  s = pm->all_songs->songs.last();
  p = s->patches.last();
  tassert(p->connections.length() == 2, "bad num conns");
  conn = p->connections.first();
  tassert(conn->input_chan == 2, "wrong conn input channel");
  tassert(conn->output_chan == 3, "wrong conn output channel");
  delete pm;
}

void test_load_bank() {
  PatchMaster *pm = load_test_file();
  Song *s = pm->all_songs->songs.first();
  Patch *p = s->patches.first();
  Connection *conn = p->connections.last();
  tassert(conn->prog.bank_msb = 3, "bad bank msb");
  tassert(conn->prog.bank_lsb = 2, "bad bank lsb");
}

void test_load_prog_chg() {
  PatchMaster *pm = load_test_file();
  Song *s = pm->all_songs->songs.first();
  Patch *p = s->patches.first();
  Connection *conn = p->connections.last();
  tassert(conn->prog.prog = 12, "bad program change");
}

void test_load_xpose() {
  PatchMaster *pm = load_test_file();
  Song *s = pm->all_songs->songs.first();
  Patch *p = s->patches.first();
  Connection *conn = p->connections.first();
  tassert(conn->xpose == 0, "bad default xpose");

  p = s->patches.last();
  conn = p->connections.first();
  tassert(conn->xpose = 12, "bad xpose");
  conn = p->connections.last();
  tassert(conn->xpose = -12, "bad xpose");
  delete pm;
}

void test_load_zone() {
  PatchMaster *pm = load_test_file();
  Song *s = pm->all_songs->songs.first();
  Patch *p = s->patches.first();
  Connection *conn = p->connections.first();
  tassert(conn->zone.low == -1, "bad default zone low");
  tassert(conn->zone.high == -1, "bad default zone high");

  s = pm->all_songs->songs[1];
  p = s->patches[1];
  conn = p->connections[0];
  tassert(conn->zone.low == 0, "bad zone low");
  tassert(conn->zone.high == 63, "bad zone high");

  conn = p->connections[1];
  tassert(conn->zone.low == 64, "bad zone low from name")
  tassert(conn->zone.high == 127, "bad zone high from name")

  delete pm;
}

void test_load_filter() {
  PatchMaster *pm = load_test_file();
  Song *s = pm->all_songs->songs.first();
  Patch *p = s->patches.first();
  Connection *conn = p->connections.last();
  tassert(conn->cc_maps[64] == -1, "bad cc filter");
  delete pm;
}

void test_load_map() {
  PatchMaster *pm = load_test_file();
  Song *s = pm->all_songs->songs.first();
  Patch *p = s->patches.first();
  Connection *conn = p->connections.first();
  tassert(conn->cc_maps[7] == 7, "bad default map");

  p = s->patches.last();
  conn = p->connections.first();
  tassert(conn->cc_maps[7] == 10, "bad cc remapping");
  delete pm;
}

void test_load_song_list() {
  PatchMaster *pm = load_test_file();
  List<Song *> &all = pm->all_songs->songs;

  tassert(pm->song_lists.length() == 3, "bad num song lists");

  SongList *sl = pm->song_lists.at(1);
  tassert(sl->name == "Song List One", "bad song list name");
  tassert(sl->songs.length() == 2, "bad num songs in song list");
  tassert(sl->songs.first() == all.first(), "wrong song");
  tassert(sl->songs.last() == all.last(), "wrong song");

  sl = pm->song_lists.last();
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
  test_run(test_load_zone);
  test_run(test_load_filter);
  test_run(test_load_map);
  test_run(test_load_song_list);
}
