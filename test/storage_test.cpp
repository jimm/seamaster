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
  SeaMaster *sm = load_test_data();

  REQUIRE(sm->inputs.size() == 2);
  REQUIRE(sm->outputs.size() == 2);

  Input *in = sm->inputs[0];
  REQUIRE(in->name == "first input");

  Output *out = sm->outputs[1];
  REQUIRE(out->name == "second output");

  delete sm;
}

TEST_CASE("load messages", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();

  REQUIRE(sm->messages.size() == 4); // includes 2 start/stop messages

  Message *msg = sm->messages[0];
  REQUIRE(msg->name == "Tune Request");
  REQUIRE(msg->messages[0] == Pm_Message(0xf6, 0, 0));

  msg = sm->messages[1];
  REQUIRE(msg->name == "Multiple Note-Offs");
  REQUIRE(msg->messages[0] == Pm_Message(0x80, 64, 0));
  REQUIRE(msg->messages[1] == Pm_Message(0x81, 64, 0));
  REQUIRE(msg->messages[2] == Pm_Message(0x82, 42, 127));

  delete sm;
}

TEST_CASE("load triggers", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();
  Trigger *t;

  REQUIRE(sm->triggers.size() == 7);

  // keys
  t = sm->triggers[0];
  REQUIRE(t->trigger_key_code == 340);
  REQUIRE(t->trigger_message == Pm_Message(0, 0, 0));
  REQUIRE(t->output_message == nullptr);
  REQUIRE(t->action == TA_PANIC);

  Input *in = sm->inputs[0];
  REQUIRE(in->triggers.size() == 5);

  t = in->triggers[0];
  REQUIRE(t->trigger_key_code == UNDEFINED);
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

  delete sm;
}

TEST_CASE("load songs", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();

  vector<Song *> &all = sm->all_songs->songs;
  REQUIRE(all.size() == 3);

  Song *s = all[0];
  REQUIRE(s->name == "Another Song");

  s = all[1];
  REQUIRE(s->name == "Song Without Explicit Patch");

  s = all[2];
  REQUIRE(s->name == "To Each His Own");

  delete sm;
}

TEST_CASE("load notes", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();

  Song *s = sm->all_songs->songs[SONG_WITHOUT_INDEX];
  REQUIRE(s->notes.size() == 0);

  s = sm->all_songs->songs[ANOTHER_INDEX];
  REQUIRE(s->notes == "this song has note text\nthat spans multiple lines");
  delete sm;
}

TEST_CASE("load patches", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();

  Song *s = sm->all_songs->songs[TO_EACH_INDEX];
  REQUIRE(s->patches.size() == 2);

  Patch *p = s->patches[0];
  REQUIRE(p->name == "Vanilla Through, Filter Two's Sustain");
  delete sm;
}

TEST_CASE("load start and stop messages", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();

  Song *s = sm->all_songs->songs[ANOTHER_INDEX];
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
  SeaMaster *sm = load_test_data();

  Song *s = sm->all_songs->songs[TO_EACH_INDEX]; // To Each His Own
  Patch *p = s->patches[0];          // Two Inputs Merging
  REQUIRE(p->connections.size() == 2);
  Connection *conn = p->connections[0];
  REQUIRE(conn->input == sm->inputs[0]);
  REQUIRE(conn->input_chan == CONNECTION_ALL_CHANNELS);
  REQUIRE(conn->output == sm->outputs[0]);
  REQUIRE(conn->output_chan == CONNECTION_ALL_CHANNELS);

  s = sm->all_songs->songs[ANOTHER_INDEX];  // Another Song
  p = s->patches.back();        // Split Into Two OUtupts
  REQUIRE(p->connections.size() == 2);
  conn = p->connections[0];
  REQUIRE(conn->input_chan == 2);
  REQUIRE(conn->output_chan == 3);
  delete sm;
}

TEST_CASE("load bank msb lsb", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();

  Song *s = sm->all_songs->songs[TO_EACH_INDEX]; // To Each His Own
  Patch *p = s->patches[0];          // Vanilla Through
  Connection *conn = p->connections.back();
  REQUIRE(conn->prog.bank_msb == 3);
  REQUIRE(conn->prog.bank_lsb == 2);
}

TEST_CASE("load bank lsb only", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();

  Song *s = sm->all_songs->songs[TO_EACH_INDEX]; // To Each His Own
  Patch *p = s->patches[1];          // One Up One Oct...
  Connection *conn = p->connections.back();
  REQUIRE(conn->prog.bank_msb == -1);
  REQUIRE(conn->prog.bank_lsb == 5);
}

TEST_CASE("load prog chg", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();

  Song *s = sm->all_songs->songs[TO_EACH_INDEX];
  Patch *p = s->patches[0];
  Connection *conn = p->connections.back();
  REQUIRE(conn->prog.prog == 12);
}

TEST_CASE("load xpose", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();

  Song *s = sm->all_songs->songs[TO_EACH_INDEX];
  Patch *p = s->patches[0];
  Connection *conn = p->connections[0];
  REQUIRE(conn->xpose == 0);

  p = s->patches.back();
  conn = p->connections[0];
  REQUIRE(conn->xpose == 12);
  conn = p->connections.back();
  REQUIRE(conn->xpose == -12);
  delete sm;
}

TEST_CASE("load zone", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();

  Song *s = sm->all_songs->songs[TO_EACH_INDEX];
  Patch *p = s->patches[0];
  Connection *conn = p->connections[0];
  REQUIRE(conn->zone.low == 0);
  REQUIRE(conn->zone.high == 127);

  s = sm->all_songs->songs[ANOTHER_INDEX];  // Another Song
  p = s->patches[1];
  conn = p->connections[0];
  REQUIRE(conn->zone.low == 0);
  REQUIRE(conn->zone.high == 63);

  conn = p->connections[1];
  REQUIRE(conn->zone.low == 64);
  REQUIRE(conn->zone.high == 127);

  delete sm;
}

TEST_CASE("load controller mappings") {
  SeaMaster *sm = load_test_data();

  Song *s = sm->all_songs->songs[TO_EACH_INDEX];
  Patch *p = s->patches[0];
  Connection *conn = p->connections.back();
  REQUIRE(conn->id() == 2);     // sanity check
  REQUIRE(conn->cc_maps[64] != nullptr);
  REQUIRE(conn->cc_maps[64]->filtered == true);

  p = s->patches[1];
  conn = p->connections.front();
  REQUIRE(conn->id() == 3);     // sanity check
  Controller *cc = conn->cc_maps[7];
  REQUIRE(cc != nullptr);
  REQUIRE(cc->translated_cc_num == 10);
  REQUIRE(cc->filtered == false);
  REQUIRE(cc->pass_through_0 == false);
  REQUIRE(cc->pass_through_127 == false);
  REQUIRE(cc->min_in() == 1);
  REQUIRE(cc->max_in() == 120);
  REQUIRE(cc->min_out() == 40);
  REQUIRE(cc->max_out() == 50);

  delete sm;
}

TEST_CASE("load song list", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();

  vector<Song *> &all = sm->all_songs->songs;

  REQUIRE(sm->set_lists.size() == 3);

  SetList *sl = sm->set_lists[1]; // first user-defined song list
  REQUIRE(sl->name == "Set List One");
  REQUIRE(sl->songs.size() == 2);
  REQUIRE(sl->songs[0] == all[TO_EACH_INDEX]);
  REQUIRE(sl->songs.back() == all[ANOTHER_INDEX]);

  sl = sm->set_lists[2];       // second user-defined song list
  REQUIRE(sl->name == "Set List Two");
  REQUIRE(sl->songs.size() == 2);
  REQUIRE(sl->songs[0] == all[ANOTHER_INDEX]);
  REQUIRE(sl->songs.back() == all[TO_EACH_INDEX]);
  delete sm;
}

TEST_CASE("load auto patch", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();

  Song *s = sm->all_songs->songs[SONG_WITHOUT_INDEX];
  REQUIRE(s->patches.size() == 1);
  Patch *p = s->patches[0];
  REQUIRE(p->name == "Default Patch");
  REQUIRE(p->connections.size() == 2);
  REQUIRE(p->connections[0]->input->name == "first input");
  REQUIRE(p->connections[1]->input->name == "second input");
}

TEST_CASE("initialize", CATCH_CATEGORY) {
  Storage storage(TEST_DB_PATH);
  storage.initialize();
  REQUIRE(storage.has_error() == false);
  
  SeaMaster *sm = storage.load(true);
  REQUIRE(sm->inputs.size() == 0);
  REQUIRE(sm->outputs.size() == 0);
  REQUIRE(sm->messages.size() == 0);
  REQUIRE(sm->all_songs->songs.size() == 0);
}

TEST_CASE("save", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();

  Storage saver(TEST_DB_PATH "_save_test");
  saver.save(sm, true);
  REQUIRE(saver.has_error() == false);

  Storage storage(TEST_DB_PATH "_save_test");
  sm = storage.load(true);
  REQUIRE(storage.has_error() == false);

  REQUIRE(sm->inputs.size() == 2);
  REQUIRE(sm->outputs.size() == 2);
  REQUIRE(sm->inputs[0]->name == "first input");
  REQUIRE(sm->messages.size() == 4);

  REQUIRE(sm->triggers.size() == 7);
  REQUIRE(sm->inputs[0]->triggers.size() == 5);

  REQUIRE(sm->all_songs->songs.size() == 3);

  Song *song = sm->all_songs->songs[0];
  REQUIRE(song->name == "Another Song");
  REQUIRE(song->patches[0]->connections.size() == 2);

  REQUIRE(sm->set_lists.size() == 3);
  REQUIRE(sm->set_lists[0] == sm->all_songs);
  REQUIRE(sm->set_lists[1]->name == "Set List One");
  REQUIRE(sm->set_lists[2]->name == "Set List Two");
}

TEST_CASE("save sets UNDEFINE_ID ids", CATCH_CATEGORY) {
  SeaMaster *sm = new SeaMaster();
  sm->testing = true;
  sm->initialize();

  Song *s = new Song(UNDEFINED_ID, "unnamed");
  sm->all_songs->songs.push_back(s);
  REQUIRE(s->id() == UNDEFINED_ID);

  Storage storage(TEST_DB_PATH);
  storage.save(sm, true);
  REQUIRE(storage.has_error() == false);
  REQUIRE(s->id() != UNDEFINED_ID);
  REQUIRE(s->id() >= 1LL);
}
