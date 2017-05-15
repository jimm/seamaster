#include "test_helper.h"
#include "patch_test.h"
#include "../src/patch.h"

void test_patch_start_msgs() {
  Patch p("test patch");
  Connection *conn = create_conn();
  p.connections.push_back(conn);
  p.start_messages.push_back(0x1234);

  p.start();
  tassert(conn->output->num_io_messages == 1, "bad num start messages");
  tassert(conn->output->io_messages[0] == 0x1234, "bad start message");
}

void test_patch_stop_msgs() {
  Patch p("test patch");
  Connection *conn = create_conn();
  p.connections.push_back(conn);
  p.stop_messages.push_back(0x1234);

  p.stop();
  tassert(conn->output->num_io_messages == 1, "bad num stop messages");
  tassert(conn->output->io_messages[0] == 0x1234, "bad stop message");
}

void test_patch() {
  test_run(test_patch_start_msgs);
  test_run(test_patch_stop_msgs);
}
