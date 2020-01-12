#include <string.h>
#include "test_helper.h"
#include "../src/loader.h"
#include "loader_test.h"

#define ORG_MODE_TEST_FILE "test/testfile.org"
#define MARKDOWN_TEST_FILE "test/testfile.md"

static const char *test_file;

void test_load_instruments() {
  PatchMaster *pm = load_test_file();
  tassert(pm->inputs.size() == 2, "bad num inputs");
  tassert(pm->outputs.size() == 2, "bad num outputs");

  Input *in = pm->inputs[0];
  tassert(in->sym == "one", "bad input sym");
  tassert(in->name == "first input", "bad input name");

  Output *out = pm->outputs[1];
  tassert(out->sym == "two", "bad output sym");
  tassert(out->name == "second output", "bad output name");

  delete pm;
}

void test_load_messages() {
  PatchMaster *pm = load_test_file();
  tassert(pm->messages.size() == 3, 0);

  Message *msg = pm->messages[0];
  tassert(msg->name == "Tune Request", 0);
  tassert(msg->messages[0] == Pm_Message(0xf6, 0, 0), 0);

  msg = pm->messages[1];
  tassert(msg->name == "Multiple Note-Offs", 0);
  tassert(msg->messages[0] == Pm_Message(0x80, 64, 0), 0);
  tassert(msg->messages[1] == Pm_Message(0x81, 64, 0), 0);
  tassert(msg->messages[2] == Pm_Message(0x82, 42, 127), 0);

  msg = pm->messages[2];
  tassert(msg->name == "Testing Another Literal Syntax", 0);
  tassert(msg->messages[0] == Pm_Message(0xf6, 0, 0), 0);

  delete pm;
}

void test_load_triggers() {
  PatchMaster *pm = load_test_file();
  Input *in = pm->inputs[0];
  tassert(in->triggers.size() == 5, 0);

  Trigger *t = in->triggers[0];
  tassert(t->trigger_message == Pm_Message(0xb0, 50, 127), 0);
  tassert(t->action == NEXT_SONG, 0);
  tassert(t->output_message == 0, 0);

  // make sure trigger added to first input
  tassert(find(in->triggers.begin(), in->triggers.end(), t) != in->triggers.end(), 0);

  t = in->triggers[4];
  tassert(t->trigger_message == Pm_Message(0xb0, 54, 127), 0);
  tassert(t->action == MESSAGE, 0);
  tassert(t->output_message != 0, 0);
  tassert(t->output_message->name == "Tune Request", 0);

  delete pm;
}

void test_load_songs() {
  PatchMaster *pm = load_test_file();
  vector<Song *> &all = pm->all_songs->songs;
  tassert(all.size() == 3, "wrong num songs loaded");

  Song *s = all[0];
  tassert(s->name == "To Each His Own", "bad song title");

  s = all[1];
  tassert(s->name == "Another Song", "bad song title");

  s = all[2];
  tassert(s->name == "Song Without Explicit Patch", 0);

  delete pm;
}

void test_load_notes() {
  PatchMaster *pm = load_test_file();
  Song *s = pm->all_songs->songs[0];
  tassert(s->notes.size() == 0, "extra notes?");

  s = pm->all_songs->songs[1];
  tassert(s->notes.size() == 3, "bad notes length");
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
  Song *s = pm->all_songs->songs[0];
  tassert(s->patches.size() == 2, "bad num patches");

  Patch *p = s->patches[0];
  tassert(p->name == "Vanilla Through, Filter Two's Sustain",
          "bad patch name");
  delete pm;
}

void test_load_start_and_stop_messages() {
  PatchMaster *pm = load_test_file();
  Song *s = pm->all_songs->songs[1];
  Patch *p = s->patches.back();

  tassert(p->start_messages.size() == 3, 0);
  tassert(p->start_messages[0] == Pm_Message(0xb0, 0x7a, 0x00), "badly-read start message 1");
  tassert(p->start_messages[1] == Pm_Message(0xb0, 7, 127), "badly-read start message 2");
  tassert(p->start_messages[2] == Pm_Message(0xb1, 7, 127), "badly-read start message 3");

  tassert(p->stop_messages.size() == 3, 0);
  tassert(p->stop_messages[0] == Pm_Message(0xb2, 7, 127), "badly-read stop message 1");
  tassert(p->stop_messages[1] == Pm_Message(0xb3, 7, 127), "badly-read stop message 2");
  tassert(p->stop_messages[2] == Pm_Message(0xb0, 0x7a, 127), "badly-read stop message 3");
}

void test_load_connections() {
  PatchMaster *pm = load_test_file();
  Song *s = pm->all_songs->songs[0];
  Patch *p = s->patches[0];
  tassert(p->connections.size() == 2, "bad num conns");
  Connection *conn = p->connections[0];
  tassert(conn->input == pm->inputs[0], "wrong conn input");
  tassert(conn->input_chan == -1, "wrong conn input channel");
  tassert(conn->output == pm->outputs[0], "wrong conn output");
  tassert(conn->output_chan == -1, "wrong conn output channel");

  s = pm->all_songs->songs[1];
  p = s->patches.back();
  tassert(p->connections.size() == 2, "bad num conns");
  conn = p->connections[0];
  tassert(conn->input_chan == 2, "wrong conn input channel");
  tassert(conn->output_chan == 3, "wrong conn output channel");
  delete pm;
}

void test_load_bank_msb_lsb() {
  PatchMaster *pm = load_test_file();
  Song *s = pm->all_songs->songs[0];
  Patch *p = s->patches[0];
  Connection *conn = p->connections.back();
  tassert(conn->prog.bank_msb == 3, "bad bank msb");
  tassert(conn->prog.bank_lsb == 2, "bad bank lsb");
}

void test_load_bank_lsb_only() {
  PatchMaster *pm = load_test_file();
  Song *s = pm->all_songs->songs[0];
  Patch *p = s->patches[1];
  Connection *conn = p->connections.back();
  tassert(conn->prog.bank_msb == -1, "bad bank msb");
  tassert(conn->prog.bank_lsb == 5, "bad bank lsb");
}

void test_load_prog_chg() {
  PatchMaster *pm = load_test_file();
  Song *s = pm->all_songs->songs[0];
  Patch *p = s->patches[0];
  Connection *conn = p->connections.back();
  tassert(conn->prog.prog == 12, "bad program change");
}

void test_load_xpose() {
  PatchMaster *pm = load_test_file();
  Song *s = pm->all_songs->songs[0];
  Patch *p = s->patches[0];
  Connection *conn = p->connections[0];
  tassert(conn->xpose == 0, "bad default xpose");

  p = s->patches.back();
  conn = p->connections[0];
  tassert(conn->xpose == 12, "bad xpose");
  conn = p->connections.back();
  tassert(conn->xpose == -12, "bad xpose");
  delete pm;
}

void test_load_zone() {
  PatchMaster *pm = load_test_file();
  Song *s = pm->all_songs->songs[0];
  Patch *p = s->patches[0];
  Connection *conn = p->connections[0];
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

void test_load_cc_filter() {
  PatchMaster *pm = load_test_file();
  Song *s = pm->all_songs->songs[0];
  Patch *p = s->patches[0];
  Connection *conn = p->connections.back();
  tassert(conn->cc_maps[64].filtered == true, "bad cc filter");
  delete pm;
}

void test_load_cc_map() {
  PatchMaster *pm = load_test_file();
  Song *s = pm->all_songs->songs[0];
  Patch *p = s->patches[0];
  Connection *conn = p->connections[0];
  tassert(conn->cc_maps[7].translated_cc_num == -1, "bad default map");

  p = s->patches.back();
  conn = p->connections[0];
  tassert(conn->cc_maps[7].translated_cc_num == 10, "bad cc remapping");
  delete pm;
}

void test_load_cc_limit() {
  PatchMaster *pm = load_test_file();
  Song *s = pm->all_songs->songs[0];
  Patch *p = s->patches[0];
  Connection *conn = p->connections[0];
  tassert(conn->cc_maps[7].min == 0, "bad default cc min");
  tassert(conn->cc_maps[7].max == 127, "bad default cc max");

  p = s->patches.back();
  conn = p->connections[0];
  tassert(conn->cc_maps[7].min == 1, "bad cc min remapping");
  tassert(conn->cc_maps[7].max == 120, "bad cc max remapping");
  delete pm;
}

void test_load_song_list() {
  PatchMaster *pm = load_test_file();
  vector<Song *> &all = pm->all_songs->songs;

  tassert(pm->song_lists.size() == 3, "bad num song lists");

  SongList *sl = pm->song_lists[1];
  tassert(sl->name == "Song List One", "bad song list name");
  tassert(sl->songs.size() == 2, "bad num songs in song list");
  tassert(sl->songs[0] == all[0], "wrong song");
  tassert(sl->songs.back() == all[1], "wrong song");

  sl = pm->song_lists[2];
  tassert(sl->name == "Song List Two", "bad song list name");
  tassert(sl->songs.size() == 2, "bad num songs in song list");
  tassert(sl->songs[0] == all[1], "wrong song");
  tassert(sl->songs.back() == all[0], "wrong song");
  delete pm;
}

void test_load_auto_patch() {
  PatchMaster *pm = load_test_file();
  Song *s = pm->all_songs->songs.back();
  tassert(s->patches.size() == 1, 0);
  Patch *p = s->patches[0];
  tassert(p->name == "Default Patch", 0);
  tassert(p->connections.size() == 2, 0);
  tassert(p->connections[0]->input->sym == "one", 0);
  tassert(p->connections[1]->input->sym == "two", 0);
}

void test_load_run_tests(const char *path) {
  test_file = path;
  test_run(test_load_instruments);
  test_run(test_load_messages);
  test_run(test_load_triggers);
  test_run(test_load_songs);
  test_run(test_load_notes);
  test_run(test_load_patches);
  test_run(test_load_start_and_stop_messages);
  test_run(test_load_connections);
  test_run(test_load_bank_msb_lsb);
  test_run(test_load_bank_lsb_only);
  test_run(test_load_prog_chg);
  test_run(test_load_xpose);
  test_run(test_load_zone);
  test_run(test_load_cc_filter);
  test_run(test_load_cc_map);
  test_run(test_load_cc_limit);
  test_run(test_load_song_list);
  test_run(test_load_auto_patch);
}

void test_load() {
  test_load_run_tests(ORG_MODE_TEST_FILE);
  test_load_run_tests(MARKDOWN_TEST_FILE);
}
