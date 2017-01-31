#include "seamaster_test.h"
#include "../src/connection.h"
#include "../src/cursor.h"
#include "connection_test.h"

Connection *create_conn(PatchMaster *pm) {
  Input *in = (Input *)list_first(pm->inputs);
  Output *out = (Output *)list_first(pm->outputs);
  in->clear();
  out->clear();
  return new Connection(in, 0, out, 0);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast"

void test_connection_start_msgs(PatchMaster *pm) {
  Connection *conn = create_conn(pm);

  PmMessage msgs[2];
  msgs[0] = Pm_Message(TUNE_REQUEST, 0, 0);
  msgs[1] = Pm_Message(CONTROLLER, CC_VOLUME_MSB, 127);

  conn->start(msgs, 2);
  tassert(conn->output->num_io_messages == 2, "bad num start messages");
  tassert(conn->output->io_messages[0] == msgs[0], "bad msg1");
  tassert(conn->output->io_messages[1] == msgs[1], "bad msg2");

  delete conn;
}

void test_connection_start_empty_msgs(PatchMaster *pm) {
  Connection *conn = create_conn(pm);

  conn->start(0, 0);
  tassert(conn->output->num_io_messages == 0, "bad num start messages");

  delete conn;
}

void test_connection_stop_msgs(PatchMaster *pm) {
  Connection *conn = create_conn(pm);

  PmMessage msgs[2];
  msgs[0] = Pm_Message(TUNE_REQUEST, 0, 0);
  msgs[1] = Pm_Message(CONTROLLER, CC_VOLUME_MSB, 127);

  conn->stop(msgs, 2);
  tassert(conn->output->num_io_messages == 2, "bad num stop messages");
  tassert(conn->output->io_messages[0] == msgs[0], "bad msg1");
  tassert(conn->output->io_messages[1] == msgs[1], "bad msg2");

  delete conn;
}

#pragma clang diagnostic pop

void test_connection_stop_empty_msgs(PatchMaster *pm) {
  Connection *conn = create_conn(pm);

  conn->stop(0, 0);
  tassert(conn->output->num_io_messages == 0, "bad num stop messages");

  delete conn;
}

void test_connection_filter_other_input_chan(PatchMaster *pm) {
  Connection *conn = create_conn(pm);
  conn->midi_in(Pm_Message(NOTE_ON + 3, 64, 127));
  tassert(conn->output->num_io_messages == 0, 0);
  delete conn;
}

void test_connection_allow_all_chans(PatchMaster *pm) {
  Connection *conn = create_conn(pm);
  conn->input_chan = -1;
  conn->midi_in(Pm_Message(NOTE_ON + 3, 64, 127));
  tassert(conn->output->num_io_messages == 1, 0);
  tassert(conn->output->io_messages[0] == Pm_Message(NOTE_ON, 64, 127), 0); /* mutated to output chan */
  delete conn;
}

void test_connection_allow_all_chans_in_and_out(PatchMaster *pm) {
  Connection *conn = create_conn(pm);
  conn->input_chan = -1;
  conn->output_chan = -1;
  conn->midi_in(Pm_Message(NOTE_ON + 3, 64, 127));
  tassert(conn->output->num_io_messages == 1, 0);
  tassert(conn->output->io_messages[0] == Pm_Message(NOTE_ON + 3, 64, 127), 0); /* out chan not changed */
  delete conn;
}

void test_connection_xpose(PatchMaster *pm) {
  Connection *conn = create_conn(pm);

  conn->midi_in(Pm_Message(NOTE_ON, 64, 127));
  conn->xpose = 12;
  conn->midi_in(Pm_Message(NOTE_ON, 64, 127));
  conn->xpose = -12;
  conn->midi_in(Pm_Message(NOTE_ON, 64, 127));

  tassert(conn->output->num_io_messages == 3, "bad num msgs");
  tassert(conn->output->io_messages[0] == Pm_Message(NOTE_ON, 64,    127), 0);
  tassert(conn->output->io_messages[1] == Pm_Message(NOTE_ON, 64+12, 127), 0);
  tassert(conn->output->io_messages[2] == Pm_Message(NOTE_ON, 64-12, 127), 0);

  delete conn;
}

void test_connection_zone(PatchMaster *pm) {
  Connection *conn = create_conn(pm);

  conn->zone.low = 0;
  conn->zone.high = 64;
  conn->midi_in(Pm_Message(NOTE_ON, 48, 127));
  conn->midi_in(Pm_Message(NOTE_OFF, 48, 127));
  conn->midi_in(Pm_Message(NOTE_ON, 76, 127));
  conn->midi_in(Pm_Message(NOTE_OFF, 76, 127));

  tassert(conn->output->num_io_messages == 2, "bad num msgs");
  tassert(conn->output->io_messages[0] == Pm_Message(NOTE_ON, 48, 127), 0);
  tassert(conn->output->io_messages[1] == Pm_Message(NOTE_OFF, 48, 127), 0);

  delete conn;
}

void test_connection_zone_poly_pressure(PatchMaster *pm) {
  Connection *conn = create_conn(pm);

  conn->zone.low = 0;
  conn->zone.high = 64;
  conn->midi_in(Pm_Message(POLY_PRESSURE, 48, 127));
  conn->midi_in(Pm_Message(POLY_PRESSURE, 76, 127));

  tassert(conn->output->num_io_messages == 1, "bad num msgs");
  tassert(conn->output->io_messages[0] == Pm_Message(POLY_PRESSURE, 48, 127), 0);

  delete conn;
}

void test_connection_filter_controller(PatchMaster *pm) {
  Connection *conn = create_conn(pm);
  conn->cc_maps[7] = -1;
  conn->midi_in(Pm_Message(CONTROLLER, 7, 127));

  tassert(conn->output->num_io_messages == 0, "bad num msgs");

  delete conn;
}

void test_connection_map_controller(PatchMaster *pm) {
  Connection *conn = create_conn(pm);
  conn->cc_maps[7] = 10;
  conn->midi_in(Pm_Message(CONTROLLER, 7, 127));

  tassert(conn->output->num_io_messages == 1, "bad num msgs");
  tassert(conn->output->io_messages[0] == Pm_Message(CONTROLLER, 10, 127), 0);

  delete conn;
}

void test_connection(PatchMaster *pm) {
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
