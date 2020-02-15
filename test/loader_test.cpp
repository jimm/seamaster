#include <string.h>
#include "catch.hpp"
#include "test_helper.h"
#include "../src/loader.h"

#define CATCH_CATEGORY "[loader]"
#define ANOTHER_SONG_INDEX 0
#define SONG_WITHOUT_PATCH_INDEX 1
#define TO_EACH_SONG_INDEX 2
#define ORG_MODE_TEST_FILE "test/testfile.org"
#define MARKDOWN_TEST_FILE "test/testfile.md"
#define WITH_ALL_TEST_FILES(var)                                        \
  auto test_file = GENERATE(ORG_MODE_TEST_FILE, MARKDOWN_TEST_FILE);    \
  PatchMaster *var = load_test_file(test_file);

vector<const char * const> test_files = {
  (const char * const)ORG_MODE_TEST_FILE,
  (const char * const)MARKDOWN_TEST_FILE
};

TEST_CASE("instruments", CATCH_CATEGORY) {
  WITH_ALL_TEST_FILES(pm);

  REQUIRE(pm->inputs.size() == 2);
  REQUIRE(pm->outputs.size() == 2);

  Input *in = pm->inputs[0];
  REQUIRE(in->sym == "one");
  REQUIRE(in->name == "first input");

  Output *out = pm->outputs[1];
  REQUIRE(out->sym == "two");
  REQUIRE(out->name == "second output");

  delete pm;
}

TEST_CASE("messages", CATCH_CATEGORY) {
  WITH_ALL_TEST_FILES(pm);

  REQUIRE(pm->messages.size() == 3);

  Message *msg = pm->messages[0];
  REQUIRE(msg->name == "Tune Request");
  REQUIRE(msg->messages[0] == Pm_Message(0xf6, 0, 0));

  msg = pm->messages[1];
  REQUIRE(msg->name == "Multiple Note-Offs");
  REQUIRE(msg->messages[0] == Pm_Message(0x80, 64, 0));
  REQUIRE(msg->messages[1] == Pm_Message(0x81, 64, 0));
  REQUIRE(msg->messages[2] == Pm_Message(0x82, 42, 127));

  msg = pm->messages[2];
  REQUIRE(msg->name == "Testing Another Literal Syntax");
  REQUIRE(msg->messages[0] == Pm_Message(0xf6, 0, 0));

  delete pm;
}

TEST_CASE("triggers", CATCH_CATEGORY) {
  WITH_ALL_TEST_FILES(pm);

  Input *in = pm->inputs[0];
  REQUIRE(in->triggers.size() == 5);

  Trigger *t = in->triggers[0];
  REQUIRE(t->trigger_message == Pm_Message(0xb0, 50, 127));
  REQUIRE(t->action == NEXT_SONG);
  REQUIRE(t->output_message == 0);

  // make sure trigger added to first input
  REQUIRE(find(in->triggers.begin(), in->triggers.end(), t) != in->triggers.end());

  t = in->triggers[4];
  REQUIRE(t->trigger_message == Pm_Message(0xb0, 54, 127));
  REQUIRE(t->action == MESSAGE);
  REQUIRE(t->output_message != 0);
  REQUIRE(t->output_message->name == "Tune Request");

  delete pm;
}

TEST_CASE("songs", CATCH_CATEGORY) {
  WITH_ALL_TEST_FILES(pm);

  vector<Song *> &all = pm->all_songs->songs;
  REQUIRE(all.size() == 3);

  Song *s = all[0];
  REQUIRE(s->name == "Another Song");

  s = all[1];
  REQUIRE(s->name == "Song Without Explicit Patch");

  s = all[2];
  REQUIRE(s->name == "To Each His Own");

  delete pm;
}

TEST_CASE("notes", CATCH_CATEGORY) {
  WITH_ALL_TEST_FILES(pm);

  Song *s = pm->all_songs->songs[TO_EACH_SONG_INDEX];
  REQUIRE(s->notes.size() == 0);

  s = pm->all_songs->songs[ANOTHER_SONG_INDEX];
  REQUIRE(s->notes.size() == 3);
  REQUIRE(strcmp((const char *)s->notes[0],
                 "the line before begin_example contains only whitespace") == 0);
  REQUIRE(strcmp((const char *)s->notes[1], "this song has note text") == 0);
  REQUIRE(strcmp((const char *)s->notes[2], "that spans multiple lines") == 0);
  delete pm;
}

TEST_CASE("patches", CATCH_CATEGORY) {
  WITH_ALL_TEST_FILES(pm);

    Song *s = pm->all_songs->songs[TO_EACH_SONG_INDEX];
    REQUIRE(s->patches.size() == 2);

    Patch *p = s->patches[0];
    REQUIRE(p->name == "Vanilla Through, Filter Two's Sustain");
    delete pm;
}

TEST_CASE("start and stop messages", CATCH_CATEGORY) {
  WITH_ALL_TEST_FILES(pm);

    Song *s = pm->all_songs->songs[ANOTHER_SONG_INDEX];
    Patch *p = s->patches.back();

    REQUIRE(p->start_messages.size() == 3);
    REQUIRE(p->start_messages[0] == Pm_Message(0xb0, 0x7a, 0x00));
    REQUIRE(p->start_messages[1] == Pm_Message(0xb0, 7, 127));
    REQUIRE(p->start_messages[2] == Pm_Message(0xb1, 7, 127));

    REQUIRE(p->stop_messages.size() == 3);
    REQUIRE(p->stop_messages[0] == Pm_Message(0xb2, 7, 127));
    REQUIRE(p->stop_messages[1] == Pm_Message(0xb3, 7, 127));
    REQUIRE(p->stop_messages[2] == Pm_Message(0xb0, 0x7a, 127));
}

TEST_CASE("connections", CATCH_CATEGORY) {
  WITH_ALL_TEST_FILES(pm);

    Song *s = pm->all_songs->songs[TO_EACH_SONG_INDEX];
    Patch *p = s->patches[0];
    REQUIRE(p->connections.size() == 2);
    Connection *conn = p->connections[0];
    REQUIRE(conn->input == pm->inputs[0]);
    REQUIRE(conn->input_chan == CONNECTION_ALL_CHANNELS);
    REQUIRE(conn->output == pm->outputs[0]);
    REQUIRE(conn->output_chan == CONNECTION_ALL_CHANNELS);

    s = pm->all_songs->songs[ANOTHER_SONG_INDEX];
    p = s->patches.back();
    REQUIRE(p->connections.size() == 2);
    conn = p->connections[0];
    REQUIRE(conn->input_chan == 2);
    REQUIRE(conn->output_chan == 3);
    delete pm;
}

TEST_CASE("bank msb lsb", CATCH_CATEGORY) {
  WITH_ALL_TEST_FILES(pm);

    Song *s = pm->all_songs->songs[TO_EACH_SONG_INDEX];
    Patch *p = s->patches[0];
    Connection *conn = p->connections.back();
    REQUIRE(conn->prog.bank_msb == 3);
    REQUIRE(conn->prog.bank_lsb == 2);
}

TEST_CASE("bank lsb only", CATCH_CATEGORY) {
  WITH_ALL_TEST_FILES(pm);

    Song *s = pm->all_songs->songs[TO_EACH_SONG_INDEX];
    Patch *p = s->patches[1];
    Connection *conn = p->connections.back();
    REQUIRE(conn->prog.bank_msb == -1);
    REQUIRE(conn->prog.bank_lsb == 5);
}

TEST_CASE("prog chg", CATCH_CATEGORY) {
  WITH_ALL_TEST_FILES(pm);

    Song *s = pm->all_songs->songs[TO_EACH_SONG_INDEX];
    Patch *p = s->patches[0];
    Connection *conn = p->connections.back();
    REQUIRE(conn->prog.prog == 12);
}

TEST_CASE("xpose", CATCH_CATEGORY) {
  WITH_ALL_TEST_FILES(pm);

    Song *s = pm->all_songs->songs[TO_EACH_SONG_INDEX];
    Patch *p = s->patches[0];
    Connection *conn = p->connections[0];
    REQUIRE(conn->xpose == 0);

    p = s->patches.back();
    conn = p->connections[0];
    REQUIRE(conn->xpose == 12);
    conn = p->connections.back();
    REQUIRE(conn->xpose == -12);
    delete pm;
}

TEST_CASE("zone", CATCH_CATEGORY) {
  WITH_ALL_TEST_FILES(pm);

    Song *s = pm->all_songs->songs[TO_EACH_SONG_INDEX];
    Patch *p = s->patches[0];
    Connection *conn = p->connections[0];
    REQUIRE(conn->zone.low == -1);
    REQUIRE(conn->zone.high == -1);

    s = pm->all_songs->songs[ANOTHER_SONG_INDEX];
    p = s->patches[1];
    conn = p->connections[0];
    REQUIRE(conn->zone.low == 0);
    REQUIRE(conn->zone.high == 63);

    conn = p->connections[1];
    REQUIRE(conn->zone.low == 64);
    REQUIRE(conn->zone.high == 127);

    delete pm;
}

TEST_CASE("cc filter") {
  WITH_ALL_TEST_FILES(pm);

  Song *s = pm->all_songs->songs[TO_EACH_SONG_INDEX];
  Patch *p = s->patches[0];
  Connection *conn = p->connections.back();
  REQUIRE(conn->cc_maps[64] != nullptr);
  REQUIRE(conn->cc_maps[64]->filtered == true);
  delete pm;
}

TEST_CASE("pass through sysex") {
  WITH_ALL_TEST_FILES(pm);

  Song *s = pm->all_songs->songs[ANOTHER_SONG_INDEX];
  Patch *p = s->patches[0];
  Connection *conn = p->connections.front();
  REQUIRE(conn->pass_through_sysex == true);

  conn = p->connections.back();
  REQUIRE(conn->pass_through_sysex == false);

  p = s->patches[1];
  conn = p->connections.front();
  REQUIRE(conn->pass_through_sysex == true);

  conn = p->connections.back();
  REQUIRE(conn->pass_through_sysex == false);
}

TEST_CASE("cc map", CATCH_CATEGORY) {
  WITH_ALL_TEST_FILES(pm);

  Song *s = pm->all_songs->songs[TO_EACH_SONG_INDEX];
  Patch *p = s->patches[0];
  Connection *conn = p->connections[0];
  REQUIRE(conn->cc_maps[7] == nullptr);

  p = s->patches.back();
  conn = p->connections[0];
  REQUIRE(conn->cc_maps[7] != nullptr);
  REQUIRE(conn->cc_maps[7]->translated_cc_num == 10);
  delete pm;
}

TEST_CASE("cc limit", CATCH_CATEGORY) {
  WITH_ALL_TEST_FILES(pm);

  Song *s = pm->all_songs->songs[TO_EACH_SONG_INDEX];
  Patch *p = s->patches[0];
  Connection *conn = p->connections[0];
  REQUIRE(conn->cc_maps[7] == nullptr);

  p = s->patches.back();
  conn = p->connections[0];
  REQUIRE(conn->cc_maps[7] != nullptr);
  REQUIRE(conn->cc_maps[7]->min == 1);
  REQUIRE(conn->cc_maps[7]->max == 120);
  delete pm;
}

TEST_CASE("song list", CATCH_CATEGORY) {
  WITH_ALL_TEST_FILES(pm);

  vector<Song *> &all = pm->all_songs->songs;

  REQUIRE(pm->song_lists.size() == 3);

  SongList *sl = pm->song_lists[1];
  REQUIRE(sl->name == "Song List One");
  REQUIRE(sl->songs.size() == 2);
  REQUIRE(sl->songs[0] == all[TO_EACH_SONG_INDEX]);
  REQUIRE(sl->songs.back() == all[ANOTHER_SONG_INDEX]);

  sl = pm->song_lists[2];
  REQUIRE(sl->name == "Song List Two");
  REQUIRE(sl->songs.size() == 2);
  REQUIRE(sl->songs[0] == all[ANOTHER_SONG_INDEX]);
  REQUIRE(sl->songs.back() == all[TO_EACH_SONG_INDEX]);
  delete pm;
}

TEST_CASE("auto patch", CATCH_CATEGORY) {
  WITH_ALL_TEST_FILES(pm);

  Song *s = pm->all_songs->songs[SONG_WITHOUT_PATCH_INDEX];
  REQUIRE(s->patches.size() == 1);
  Patch *p = s->patches[0];
  REQUIRE(p->name == "Default Patch");
  REQUIRE(p->connections.size() == 2);
  REQUIRE(p->connections[0]->input->sym == "one");
  REQUIRE(p->connections[1]->input->sym == "two");
}
