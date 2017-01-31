#include "test_helper.h"

PatchMaster *create_pm() {
  PatchMaster *pm = new PatchMaster();
  pm->testing = true;
  return pm;
}

Connection *create_conn() {
  Input *in = new Input("in", "input name", -1);
  Output *out = new Output("out", "output name", -1);
  Connection *conn = new Connection(in, 0, out, 0);
  conn->start(0, 0);            // add conn to input
  return conn;
}
