#include "test_helper.h"
#include "connection_test.h"

void test_connection_start_sends_pc() {
  Input *in = new Input("in", "input name", -1);
  Output *out = new Output("out", "output name", 1);
  Connection *conn = new Connection(in, 0, out, 0);
  vector<PmMessage> empty;

  conn->prog.prog = 123;
  conn->start();
  tassert(conn->output->num_io_messages == 1, 0);
  tassert(conn->output->io_messages[0] == Pm_Message(PROGRAM_CHANGE + 1, 123, 0), 0);
  delete conn;
}

void test_connection_filter_other_input_chan() {
  Connection *conn = create_conn();
  conn->midi_in(Pm_Message(NOTE_ON + 3, 64, 127));
  tassert(conn->output->num_io_messages == 0, 0);
  delete conn;
}

void test_connection_allow_all_chans() {
  Connection *conn = create_conn();
  conn->input_chan = -1;
  conn->midi_in(Pm_Message(NOTE_ON + 3, 64, 127));
  tassert(conn->output->num_io_messages == 1, 0);
  tassert(conn->output->io_messages[0] == Pm_Message(NOTE_ON, 64, 127), 0); /* mutated to output chan */
  delete conn;
}

void test_connection_allow_all_chans_in_and_out() {
  Connection *conn = create_conn();
  conn->input_chan = -1;
  conn->output_chan = -1;
  conn->midi_in(Pm_Message(NOTE_ON + 3, 64, 127));
  tassert(conn->output->num_io_messages == 1, 0);
  tassert(conn->output->io_messages[0] == Pm_Message(NOTE_ON + 3, 64, 127), 0); /* out chan not changed */
  delete conn;
}

void test_connection_all_chans_filter_controller() {
  Connection *conn = create_conn();
  conn->input_chan = -1;
  conn->output_chan = -1;
  conn->cc_maps[64].filtered = true;
  conn->midi_in(Pm_Message(CONTROLLER + 3, 64, 127));
  tassert(conn->output->num_io_messages == 0, "should not pass through filtered controller");
  delete conn;
}

void test_connection_all_chans_process_controller() {
  Connection *conn = create_conn();
  conn->input_chan = 3;
  conn->output_chan = 3;
  conn->cc_maps[64].max = 126;
  conn->midi_in(Pm_Message(CONTROLLER + 3, 64, 127));
  tassert(conn->output->num_io_messages == 1, 0);
  tassert(conn->output->io_messages[0] == Pm_Message(NOTE_ON + 3, 64, 126), 0); /* out value clamped */
  delete conn;
}

void test_connection_xpose() {
  Connection *conn = create_conn();

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

void test_connection_zone() {
  Connection *conn = create_conn();

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

void test_connection_zone_poly_pressure() {
  Connection *conn = create_conn();

  conn->zone.low = 0;
  conn->zone.high = 64;
  conn->midi_in(Pm_Message(POLY_PRESSURE, 48, 127));
  conn->midi_in(Pm_Message(POLY_PRESSURE, 76, 127));

  tassert(conn->output->num_io_messages == 1, "bad num msgs");
  tassert(conn->output->io_messages[0] == Pm_Message(POLY_PRESSURE, 48, 127), 0);

  delete conn;
}

void test_connection_cc_processed() {
  Connection *conn = create_conn();
  conn->cc_maps[7].filtered = true;
  conn->midi_in(Pm_Message(CONTROLLER, 7, 127));

  tassert(conn->output->num_io_messages == 0, "bad num msgs");

  delete conn;
}

void test_connection() {
  test_run(test_connection_filter_other_input_chan);
  test_run(test_connection_allow_all_chans);
  test_run(test_connection_allow_all_chans_in_and_out);
  test_run(test_connection_all_chans_filter_controller);
  test_run(test_connection_xpose);
  test_run(test_connection_zone);
  test_run(test_connection_zone_poly_pressure);
  test_run(test_connection_cc_processed);
}
