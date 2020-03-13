#include <string.h>
#include "catch.hpp"
#include "test_helper.h"
#include "../src/storage.h"

#define CATCH_CATEGORY "[storage]"
// These are the indices of songs in the all-songs list. Different than
// database ID order because the all-songs list is sorted by song name.
#define ANOTHER_INDEX 0
#define SONG_WITHOUT_INDEX 1
#define TO_EACH_INDEX 2

TEST_CASE("load instruments", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();

  REQUIRE(pm->inputs.size() == 2);
  REQUIRE(pm->outputs.size() == 2);

  Input *in = pm->inputs[0];
  REQUIRE(in->name == "first input");

  Output *out = pm->outputs[1];
  REQUIRE(out->name == "second output");

  delete pm;
}

TEST_CASE("load messages", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();

  REQUIRE(pm->messages.size() == 4); // includes 2 start/stop messages

  Message *msg = pm->messages[0];
  REQUIRE(msg->name == "Tune Request");
  REQUIRE(msg->messages[0] == Pm_Message(0xf6, 0, 0));

  msg = pm->messages[1];
  REQUIRE(msg->name == "Multiple Note-Offs");
  REQUIRE(msg->messages[0] == Pm_Message(0x80, 64, 0));
  REQUIRE(msg->messages[1] == Pm_Message(0x81, 64, 0));
  REQUIRE(msg->messages[2] == Pm_Message(0x82, 42, 127));

  delete pm;
}

TEST_CASE("load triggers", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();

  Input *in = pm->inputs[0];
  REQUIRE(in->triggers.size() == 5);

  Trigger *t = in->triggers[0];
  REQUIRE(t->trigger_message == Pm_Message(0xb0, 50, 127));
  REQUIRE(t->action == TA_NEXT_SONG);
  REQUIRE(t->output_message == 0);

  // make sure trigger added to first input
  REQUIRE(find(in->triggers.begin(), in->triggers.end(), t) != in->triggers.end());

  t = in->triggers[4];
  REQUIRE(t->trigger_message == Pm_Message(0xb0, 54, 127));
  REQUIRE(t->action == TA_MESSAGE);
  REQUIRE(t->output_message != 0);
  REQUIRE(t->output_message->name == "Tune Request");

  delete pm;
}

TEST_CASE("load songs", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();

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

TEST_CASE("load notes", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();

  Song *s = pm->all_songs->songs[SONG_WITHOUT_INDEX];
  REQUIRE(s->notes.size() == 0);

  s = pm->all_songs->songs[ANOTHER_INDEX];
  REQUIRE(s->notes == "this song has note text\nthat spans multiple lines");
  delete pm;
}

TEST_CASE("load patches", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();

  Song *s = pm->all_songs->songs[TO_EACH_INDEX];
  REQUIRE(s->patches.size() == 2);

  Patch *p = s->patches[0];
  REQUIRE(p->name == "Vanilla Through, Filter Two's Sustain");
  delete pm;
}

TEST_CASE("load start and stop messages", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();

  Song *s = pm->all_songs->songs[ANOTHER_INDEX];
  Patch *p = s->patches.back();

  REQUIRE(p->start_message->messages.size() == 3);
  REQUIRE(p->start_message->messages[0] == Pm_Message(0xb0, 0x7a, 0x00));
  REQUIRE(p->start_message->messages[1] == Pm_Message(0xb0, 7, 127));
  REQUIRE(p->start_message->messages[2] == Pm_Message(0xb1, 7, 127));

  REQUIRE(p->stop_message->messages.size() == 3);
  REQUIRE(p->stop_message->messages[0] == Pm_Message(0xb2, 7, 127));
  REQUIRE(p->stop_message->messages[1] == Pm_Message(0xb3, 7, 127));
  REQUIRE(p->stop_message->messages[2] == Pm_Message(0xb0, 0x7a, 127));
}

TEST_CASE("load connections", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();

  Song *s = pm->all_songs->songs[TO_EACH_INDEX]; // To Each His Own
  Patch *p = s->patches[0];          // Two Inputs Merging
  REQUIRE(p->connections.size() == 2);
  Connection *conn = p->connections[0];
  REQUIRE(conn->input == pm->inputs[0]);
  REQUIRE(conn->input_chan == CONNECTION_ALL_CHANNELS);
  REQUIRE(conn->output == pm->outputs[0]);
  REQUIRE(conn->output_chan == CONNECTION_ALL_CHANNELS);

  s = pm->all_songs->songs[ANOTHER_INDEX];  // Another Song
  p = s->patches.back();        // Split Into Two OUtupts
  REQUIRE(p->connections.size() == 2);
  conn = p->connections[0];
  REQUIRE(conn->input_chan == 2);
  REQUIRE(conn->output_chan == 3);
  delete pm;
}

TEST_CASE("load bank msb lsb", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();

  Song *s = pm->all_songs->songs[TO_EACH_INDEX]; // To Each His Own
  Patch *p = s->patches[0];          // Vanilla Through
  Connection *conn = p->connections.back();
  REQUIRE(conn->prog.bank_msb == 3);
  REQUIRE(conn->prog.bank_lsb == 2);
}

TEST_CASE("load bank lsb only", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();

  Song *s = pm->all_songs->songs[TO_EACH_INDEX]; // To Each His Own
  Patch *p = s->patches[1];          // One Up One Oct...
  Connection *conn = p->connections.back();
  REQUIRE(conn->prog.bank_msb == -1);
  REQUIRE(conn->prog.bank_lsb == 5);
}

TEST_CASE("load prog chg", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();

  Song *s = pm->all_songs->songs[TO_EACH_INDEX];
  Patch *p = s->patches[0];
  Connection *conn = p->connections.back();
  REQUIRE(conn->prog.prog == 12);
}

TEST_CASE("load xpose", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();

  Song *s = pm->all_songs->songs[TO_EACH_INDEX];
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

TEST_CASE("load zone", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();

  Song *s = pm->all_songs->songs[TO_EACH_INDEX];
  Patch *p = s->patches[0];
  Connection *conn = p->connections[0];
  REQUIRE(conn->zone.low == 0);
  REQUIRE(conn->zone.high == 127);

  s = pm->all_songs->songs[ANOTHER_INDEX];  // Another Song
  p = s->patches[1];
  conn = p->connections[0];
  REQUIRE(conn->zone.low == 0);
  REQUIRE(conn->zone.high == 63);

  conn = p->connections[1];
  REQUIRE(conn->zone.low == 64);
  REQUIRE(conn->zone.high == 127);

  delete pm;
}

TEST_CASE("load controller mappings") {
  PatchMaster *pm = load_test_data();

  Song *s = pm->all_songs->songs[TO_EACH_INDEX];
  Patch *p = s->patches[0];
  Connection *conn = p->connections.back();
  REQUIRE(conn->id() == 2);     // sanity check
  REQUIRE(conn->cc_maps[64] != nullptr);
  REQUIRE(conn->cc_maps[64]->filtered == true);

  p = s->patches[1];
  conn = p->connections.front();
  REQUIRE(conn->id() == 3);     // sanity check
  REQUIRE(conn->cc_maps[7] != nullptr);
  REQUIRE(conn->cc_maps[7]->translated_cc_num == 10);
  REQUIRE(conn->cc_maps[7]->min == 1);
  REQUIRE(conn->cc_maps[7]->max == 120);
  REQUIRE(conn->cc_maps[7]->filtered == false);

  delete pm;
}

TEST_CASE("load song list", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();

  vector<Song *> &all = pm->all_songs->songs;

  REQUIRE(pm->set_lists.size() == 3);

  SetList *sl = pm->set_lists[1]; // first user-defined song list
  REQUIRE(sl->name == "Set List One");
  REQUIRE(sl->songs.size() == 2);
  REQUIRE(sl->songs[0] == all[TO_EACH_INDEX]);
  REQUIRE(sl->songs.back() == all[ANOTHER_INDEX]);

  sl = pm->set_lists[2];       // second user-defined song list
  REQUIRE(sl->name == "Set List Two");
  REQUIRE(sl->songs.size() == 2);
  REQUIRE(sl->songs[0] == all[ANOTHER_INDEX]);
  REQUIRE(sl->songs.back() == all[TO_EACH_INDEX]);
  delete pm;
}

TEST_CASE("load auto patch", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();

  Song *s = pm->all_songs->songs[SONG_WITHOUT_INDEX];
  REQUIRE(s->patches.size() == 1);
  Patch *p = s->patches[0];
  REQUIRE(p->name == "Default Patch");
  REQUIRE(p->connections.size() == 2);
  REQUIRE(p->connections[0]->input->name == "first input");
  REQUIRE(p->connections[1]->input->name == "second input");
}

TEST_CASE("initialize", CATCH_CATEGORY) {
  Storage storage(TEST_DB_PATH);
  storage.initialize(true);
  REQUIRE(storage.has_error() == false);
  
  PatchMaster *pm = storage.load(true);
  REQUIRE(pm->inputs.size() == 0);
  REQUIRE(pm->outputs.size() == 0);
  REQUIRE(pm->messages.size() == 0);
  REQUIRE(pm->all_songs->songs.size() == 0);
}

TEST_CASE("save", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();

  Storage storage(TEST_DB_PATH);
  storage.save(pm, true);
  REQUIRE(storage.has_error() == false);

  pm = storage.load(true);
  REQUIRE(storage.has_error() == false);
  REQUIRE(pm->inputs.size() == 2);
  REQUIRE(pm->outputs.size() == 2);
  REQUIRE(pm->inputs[0]->name == "first input");
  REQUIRE(pm->messages.size() == 4);
  REQUIRE(pm->inputs[0]->triggers.size() == 5);
  REQUIRE(pm->all_songs->songs.size() == 3);
  REQUIRE(pm->all_songs->songs[0]->name == "Another Song");

  REQUIRE(pm->set_lists.size() == 3);
  REQUIRE(pm->set_lists[0] == pm->all_songs);
  REQUIRE(pm->set_lists[1]->name == "Set List One");
  REQUIRE(pm->set_lists[2]->name == "Set List Two");
}

TEST_CASE("save sets UNDEFINE_ID ids", CATCH_CATEGORY) {
  PatchMaster *pm = new PatchMaster();
  pm->testing = true;
  pm->initialize();

  Song *s = new Song(UNDEFINED_ID, "unnamed");
  pm->all_songs->songs.push_back(s);
  REQUIRE(s->id() == UNDEFINED_ID);

  Storage storage(TEST_DB_PATH);
  storage.save(pm, true);
  REQUIRE(storage.has_error() == false);
  REQUIRE(s->id() != UNDEFINED_ID);
  REQUIRE(s->id() >= 1LL);
}
