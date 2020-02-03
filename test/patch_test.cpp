#include "catch.hpp"
#include "test_helper.h"
#include "../src/patch.h"

#define CATCH_CATEGORY "[patch]"

TEST_CASE("start msgs", CATCH_CATEGORY) {
  Patch p("test patch");
  Connection *conn = create_conn();
  p.connections.push_back(conn);
  p.start_messages.push_back(0x1234);

  p.start();
  REQUIRE(conn->output->num_io_messages == 1);
  REQUIRE(conn->output->io_messages[0] == 0x1234);
}

TEST_CASE("stop msgs", CATCH_CATEGORY) {
  Patch p("test patch");
  Connection *conn = create_conn();
  p.connections.push_back(conn);
  p.stop_messages.push_back(0x1234);

  p.running = true;
  p.stop();
  REQUIRE(conn->output->num_io_messages == 1);
  REQUIRE(conn->output->io_messages[0] == 0x1234);
}
