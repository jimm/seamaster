#include <stdlib.h>
#include "test_helper.h"
#include "input_test.h"

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

void test_input_through_connection() {
  Connection *conn = create_conn();
  Input *in = conn->input;
  Output *out = conn->output;
  PmMessage *buf = test_events();

  for (int i = 0; i < 4; ++i)
    in->read(buf[i]);

  tassert(in->num_io_messages == 4, BAD_INPUT_COUNT);
  tassert(out->num_io_messages == 4, BAD_OUTPUT_COUNT);
  for (int i = 0; i < 4; ++i) {
    tassert(in->io_messages[i] == buf[i], BAD_INPUT);
    tassert(out->io_messages[i] == buf[i], BAD_OUTPUT);
  }

  free(buf);
  delete conn;
}

void test_input_two_connections() {
  Connection *conn = create_conn();
  Input *in = conn->input;
  Output *out = conn->output;

  Output *out2 = new Output("out2", "output2 name", -1);
  Connection *conn2 = new Connection(in, 0, out2, 0);
  conn2->start();

  PmMessage *buf = test_events();
  for (int i = 0; i < 4; ++i)
    in->read(buf[i]);

  tassert(in->num_io_messages == 4, BAD_INPUT_COUNT);
  tassert(out->num_io_messages == 4, BAD_OUTPUT_COUNT);
  tassert(out2->num_io_messages == 4, BAD_OUTPUT_COUNT);
  for (int i = 0; i < 4; ++i) {
    tassert(in->io_messages[i] == buf[i], BAD_INPUT);
    tassert(out->io_messages[i] == buf[i], BAD_OUTPUT);
    tassert(out2->io_messages[i] == buf[i], BAD_OUTPUT);
  }

  free(buf);
  delete conn;
}

void test_input_connection_switch_routes_offs_correctly() {
  Connection *conn = create_conn();
  Input *in = conn->input;
  Output *out = conn->output;

  Output *out2 = new Output("out2", "output2 name", -1);
  Connection *conn2 = new Connection(in, 0, out2, 0);

  PmMessage *buf = test_events();

  for (int i = 0; i < 2; ++i)
    in->read(buf[i]);           // note on, controller
  conn->stop();
  conn2->start();
  for (int i = 2; i < 4; ++i)
    in->read(buf[i]);           // note off, tune request

  // Make sure note off was sent to original output
  tassert(in->num_io_messages == 4, BAD_INPUT_COUNT);
  tassert(out->num_io_messages == 3, BAD_OUTPUT_COUNT);
  tassert(out2->num_io_messages == 1, BAD_OUTPUT_COUNT);
  for (int i = 0; i < 4; ++i)
    tassert(in->io_messages[i] == buf[i], BAD_INPUT);

  for (int i = 0; i < 3; ++i)
    tassert(out->io_messages[i] == buf[i], BAD_OUTPUT);

  tassert(out2->io_messages[0] == buf[3], BAD_OUTPUT);

  free(buf);
  delete conn;
}

void test_input_connection_switch_sustains_correctly() {
  Connection *conn = create_conn();
  Input *in = conn->input;
  Output *out = conn->output;

  Output *out2 = new Output("out2", "output2 name", -1);
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
  tassert(in->num_io_messages == 4, BAD_INPUT_COUNT);
  tassert(out->num_io_messages == 4, BAD_OUTPUT_COUNT);
  tassert(out2->num_io_messages == 0, BAD_OUTPUT_COUNT);
  for (int i = 0; i < 4; ++i)
    tassert(in->io_messages[i] == buf[i], BAD_INPUT);

  for (int i = 0; i < 4; ++i)
    tassert(out->io_messages[i] == buf[i], BAD_OUTPUT);

  delete conn;
}

void test_input() {
  test_run(test_input_through_connection);
  test_run(test_input_two_connections);
  test_run(test_input_connection_switch_routes_offs_correctly);
  test_run(test_input_connection_switch_sustains_correctly);
}
