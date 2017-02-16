#include "test_helper.h"

Connection *create_conn() {
  Input *in = new Input("in", "input name", -1);
  Output *out = new Output("out", "output name", -1);
  Connection *conn = new Connection(in, 0, out, 0);
  List<PmMessage> empty;
  conn->start(empty);           // add conn to input
  return conn;
}
