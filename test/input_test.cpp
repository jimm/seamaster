#include "catch.hpp"
#include "test_helper.h"

#define CATCH_CATEGORY "[input]"

const char *BAD_INPUT_COUNT = "bad num input messages";
const char *BAD_OUTPUT_COUNT = "bad num output messages";
const char *BAD_INPUT = "wrong input recorded";
const char *BAD_OUTPUT = "wrong output recorded";

PmMessage *test_events() {
  PmMessage *buf = (PmMessage *)malloc(4 * sizeof(PmMessage));
  buf[0] = Pm_Message(NOTE_ON, 64, 127);
  buf[1] = Pm_Message(CONTROLLER, 7, 127);
  buf[2] = Pm_Message(NOTE_OFF, 64, 127);
  buf[3] = Pm_Message(TUNE_REQUEST, 0, 0);
  return buf;
}

TEST_CASE("through connection", CATCH_CATEGORY) {
  Connection *conn = create_conn();
  Input *in = conn->input;
  Output *out = conn->output;
  PmMessage *buf = test_events();

  for (int i = 0; i < 4; ++i)
    in->read(buf[i]);

  REQUIRE(in->num_io_messages == 4);
  REQUIRE(out->num_io_messages == 4);
  for (int i = 0; i < 4; ++i) {
    REQUIRE(in->io_messages[i] == buf[i]);
    REQUIRE(out->io_messages[i] == buf[i]);
  }

  free(buf);
  delete conn;
}

TEST_CASE("two connections", CATCH_CATEGORY) {
  Connection *conn = create_conn();
  Input *in = conn->input;
  Output *out = conn->output;

  Output *out2 = new Output("out2", "output2 name", CONNECTION_ALL_CHANNELS);
  Connection *conn2 = new Connection(in, 0, out2, 0);
  conn2->start();

  PmMessage *buf = test_events();
  for (int i = 0; i < 4; ++i)
    in->read(buf[i]);

  REQUIRE(in->num_io_messages == 4);
  REQUIRE(out->num_io_messages == 4);
  REQUIRE(out2->num_io_messages == 4);
  for (int i = 0; i < 4; ++i) {
    REQUIRE(in->io_messages[i] == buf[i]);
    REQUIRE(out->io_messages[i] == buf[i]);
    REQUIRE(out2->io_messages[i] == buf[i]);
  }

  free(buf);
  delete conn;
}

TEST_CASE("connection switch routes offs correctly", CATCH_CATEGORY) {
  Connection *conn = create_conn();
  Input *in = conn->input;
  Output *out = conn->output;

  Output *out2 = new Output("out2", "output2 name", CONNECTION_ALL_CHANNELS);
  Connection *conn2 = new Connection(in, 0, out2, 0);

  PmMessage *buf = test_events();

  for (int i = 0; i < 2; ++i)
    in->read(buf[i]);           // note on, controller
  conn->stop();
  conn2->start();
  for (int i = 2; i < 4; ++i)
    in->read(buf[i]);           // note off, tune request

  // Make sure note off was sent to original output
  REQUIRE(in->num_io_messages == 4);
  REQUIRE(out->num_io_messages == 3);
  REQUIRE(out2->num_io_messages == 1);
  for (int i = 0; i < 4; ++i)
    REQUIRE(in->io_messages[i] == buf[i]);

  for (int i = 0; i < 3; ++i)
    REQUIRE(out->io_messages[i] == buf[i]);

  REQUIRE(out2->io_messages[0] == buf[3]);

  free(buf);
  delete conn;
}

TEST_CASE("connection switch sustains correctly", CATCH_CATEGORY) {
  Connection *conn = create_conn();
  Input *in = conn->input;
  Output *out = conn->output;

  Output *out2 = new Output("out2", "output2 name", CONNECTION_ALL_CHANNELS);
  Connection *conn2 = new Connection(in, 0, out2, 0);

  PmMessage buf[4] = {
    Pm_Message(NOTE_ON, 64, 127),
    Pm_Message(CONTROLLER, CC_SUSTAIN, 127),
    Pm_Message(NOTE_OFF, 64, 127),
    Pm_Message(CONTROLLER, CC_SUSTAIN, 0)
  };

  for (int i = 0; i < 2; ++i)
    in->read(buf[i]);           // note on, sustain on
  conn->stop();
  conn2->start();
  for (int i = 2; i < 4; ++i)
    in->read(buf[i]);           // note off, sustain off

  // Make sure note off was sent to original output
  REQUIRE(in->num_io_messages == 4);
  REQUIRE(out->num_io_messages == 4);
  REQUIRE(out2->num_io_messages == 0);
  for (int i = 0; i < 4; ++i)
    REQUIRE(in->io_messages[i] == buf[i]);

  for (int i = 0; i < 4; ++i)
    REQUIRE(out->io_messages[i] == buf[i]);

  delete conn;
}
