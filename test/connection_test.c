#include "seamaster_test.h"
#include "../src/connection.h"
#include "../src/cursor.h"
#include "connection_test.h"

connection *create_conn(patchmaster *pm) {
  input *in = list_first(pm->inputs);
  output *out = list_first(pm->outputs);
  input_clear(in);
  output_clear(out);
  return connection_new(in, 0, out, 0);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast"

void test_connection_start_msgs(patchmaster *pm) {
  connection *conn = create_conn(pm);

  list *msgs = list_new();
  PmMessage msg1 = Pm_Message(TUNE_REQUEST, 0, 0);
  PmMessage msg2 = Pm_Message(CONTROLLER, CC_VOLUME_MSB, 127);
  list_append(msgs, (void *)msg1);
  list_append(msgs, (void *)msg2);

  connection_start(conn, msgs);
  tassert(conn->output->num_sent_messages == 2, "bad num start messages");
  tassert(conn->output->sent_messages[0] == msg1, "bad msg1");
  tassert(conn->output->sent_messages[1] == msg2, "bad msg2");

  list_free(msgs, 0);
  connection_free(conn);
}

void test_connection_start_empty_msgs(patchmaster *pm) {
  connection *conn = create_conn(pm);

  list *msgs = list_new();
  connection_start(conn, msgs);
  tassert(conn->output->num_sent_messages == 0, "bad num start messages");

  list_free(msgs, 0);
  connection_free(conn);
}

void test_connection_stop_msgs(patchmaster *pm) {
  connection *conn = create_conn(pm);

  list *msgs = list_new();
  PmMessage msg1 = Pm_Message(TUNE_REQUEST, 0, 0);
  PmMessage msg2 = Pm_Message(CONTROLLER, CC_VOLUME_MSB, 127);
  list_append(msgs, (void *)msg1);
  list_append(msgs, (void *)msg2);

  connection_stop(conn, msgs);
  tassert(conn->output->num_sent_messages == 2, "bad num stop messages");
  tassert(conn->output->sent_messages[0] == msg1, "bad msg1");
  tassert(conn->output->sent_messages[1] == msg2, "bad msg2");

  list_free(msgs, 0);
  connection_free(conn);
}

#pragma clang diagnostic pop

void test_connection_stop_empty_msgs(patchmaster *pm) {
  connection *conn = create_conn(pm);

  list *msgs = list_new();
  connection_stop(conn, msgs);
  tassert(conn->output->num_sent_messages == 0, "bad num stop messages");

  list_free(msgs, 0);
  connection_free(conn);
}

void test_connection_filter_other_input_chan(patchmaster *pm) {
  connection *conn = create_conn(pm);
  connection_midi_in(conn, Pm_Message(NOTE_ON + 3, 64, 127));
  tassert(conn->output->num_sent_messages == 0, 0);
  connection_free(conn);
}

void test_connection_allow_all_chans(patchmaster *pm) {
  connection *conn = create_conn(pm);
  conn->input_chan = -1;
  connection_midi_in(conn, Pm_Message(NOTE_ON + 3, 64, 127));
  tassert(conn->output->num_sent_messages == 1, 0);
  tassert(conn->output->sent_messages[0] == Pm_Message(NOTE_ON, 64, 127), 0); /* mutated to output chan */
  connection_free(conn);
}

void test_connection_allow_all_chans_in_and_out(patchmaster *pm) {
  connection *conn = create_conn(pm);
  conn->input_chan = -1;
  conn->output_chan = -1;
  connection_midi_in(conn, Pm_Message(NOTE_ON + 3, 64, 127));
  tassert(conn->output->num_sent_messages == 1, 0);
  tassert(conn->output->sent_messages[0] == Pm_Message(NOTE_ON + 3, 64, 127), 0); /* out chan not changed */
  connection_free(conn);
}

void test_connection_xpose(patchmaster *pm) {
  connection *conn = create_conn(pm);

  connection_midi_in(conn, Pm_Message(NOTE_ON, 64, 127));
  conn->xpose = 12;
  connection_midi_in(conn, Pm_Message(NOTE_ON, 64, 127));
  conn->xpose = -12;
  connection_midi_in(conn, Pm_Message(NOTE_ON, 64, 127));

  tassert(conn->output->num_sent_messages == 3, "bad num msgs");
  tassert(conn->output->sent_messages[0] == Pm_Message(NOTE_ON, 64,    127), 0);
  tassert(conn->output->sent_messages[1] == Pm_Message(NOTE_ON, 64+12, 127), 0);
  tassert(conn->output->sent_messages[2] == Pm_Message(NOTE_ON, 64-12, 127), 0);

  connection_free(conn);
}

void test_connection_zone(patchmaster *pm) {
  connection *conn = create_conn(pm);

  conn->zone.low = 0;
  conn->zone.high = 64;
  connection_midi_in(conn, Pm_Message(NOTE_ON, 48, 127));
  connection_midi_in(conn, Pm_Message(NOTE_OFF, 48, 127));
  connection_midi_in(conn, Pm_Message(NOTE_ON, 76, 127));
  connection_midi_in(conn, Pm_Message(NOTE_OFF, 76, 127));

  tassert(conn->output->num_sent_messages == 2, "bad num msgs");
  tassert(conn->output->sent_messages[0] == Pm_Message(NOTE_ON, 48, 127), 0);
  tassert(conn->output->sent_messages[1] == Pm_Message(NOTE_OFF, 48, 127), 0);

  connection_free(conn);
}

void test_connection_zone_poly_pressure(patchmaster *pm) {
  connection *conn = create_conn(pm);

  conn->zone.low = 0;
  conn->zone.high = 64;
  connection_midi_in(conn, Pm_Message(POLY_PRESSURE, 48, 127));
  connection_midi_in(conn, Pm_Message(POLY_PRESSURE, 76, 127));

  tassert(conn->output->num_sent_messages == 1, "bad num msgs");
  tassert(conn->output->sent_messages[0] == Pm_Message(POLY_PRESSURE, 48, 127), 0);

  connection_free(conn);
}

void test_connection_filter_controller(patchmaster *pm) {
  connection *conn = create_conn(pm);
  conn->cc_maps[7] = -1;
  connection_midi_in(conn, Pm_Message(CONTROLLER, 7, 127));

  tassert(conn->output->num_sent_messages == 0, "bad num msgs");

  connection_free(conn);
}

void test_connection_map_controller(patchmaster *pm) {
  connection *conn = create_conn(pm);
  conn->cc_maps[7] = 10;
  connection_midi_in(conn, Pm_Message(CONTROLLER, 7, 127));

  tassert(conn->output->num_sent_messages == 1, "bad num msgs");
  tassert(conn->output->sent_messages[0] == Pm_Message(CONTROLLER, 10, 127), 0);

  connection_free(conn);
}

void test_connection(patchmaster *pm) {
  test_run(test_connection_start_msgs);
  test_run(test_connection_start_empty_msgs);
  test_run(test_connection_stop_msgs);
  test_run(test_connection_stop_empty_msgs);
  test_run(test_connection_filter_other_input_chan);
  test_run(test_connection_allow_all_chans);
  test_run(test_connection_allow_all_chans_in_and_out);
  test_run(test_connection_xpose);
  test_run(test_connection_zone);
  test_run(test_connection_zone_poly_pressure);
  test_run(test_connection_filter_controller);
  test_run(test_connection_map_controller);
}
