#include "test_helper.h"
#include "../src/loader.h"

PatchMaster *load_test_file() {
  return load_test_file("test/testfile.org");
}

PatchMaster *load_test_file(const char *path) {
  PatchMaster *old_pm = PatchMaster_instance();
  if (old_pm)
    delete old_pm;

  Loader loader;
  PatchMaster *pm = loader.load(path, true);
  pm->testing = true;
  return pm;
}

Connection *create_conn() {
  Input *in = new Input("in", "input name", "in port name", CONNECTION_ALL_CHANNELS);
  Output *out = new Output("out", "output name", "out port name", CONNECTION_ALL_CHANNELS);
  Connection *conn = new Connection(in, 0, out, 0);
  vector<PmMessage> empty;
  conn->start();                // add conn to input
  return conn;
}
