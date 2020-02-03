#include "catch.hpp"
#include "../src/patchmaster.h"

#define CATCH_CATEGORY "[patchmaster]"
#include "test_helper.h"

void assert_no_start_sent(PatchMaster *pm) {
  for (auto& out : pm->outputs) {
    for (int i = 0; i < out->num_io_messages; ++i)
      if (out->io_messages[i] == Pm_Message(0xb0, 7, 127))
        REQUIRE("assert no start sent" == "start was sent");
  }
  REQUIRE(true);
}

void assert_no_stop_sent(PatchMaster *pm) {
  for (auto& out : pm->outputs) {
    for (int i = 0; i < out->num_io_messages; ++i)
      if (out->io_messages[i] == Pm_Message(0xb2, 7, 127))
        REQUIRE("assert no stop sent" == "stop was sent");
  }
  REQUIRE(true);
}

void assert_start_sent(PatchMaster *pm) {
  for (auto& out : pm->outputs) {
    for (int i = 0; i < out->num_io_messages; ++i)
      if (out->io_messages[i] == Pm_Message(0xb0, 7, 127))
        return;
  }
  REQUIRE("assert start sent" == "start was not sent");
}

void assert_stop_sent(PatchMaster *pm) {
  Output *out = pm->outputs[0];
  for (int i = 0; i < out->num_io_messages; ++i)
    if (out->io_messages[i] == Pm_Message(0xb2, 7, 127))
      return;
  REQUIRE("assert stop sent" == "stop message not sent");
}

TEST_CASE("next patch start and stop messages", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_file();
  pm->start();

  pm->next_song();
  assert_no_start_sent(pm);
  assert_no_stop_sent(pm);

  pm->next_patch();
  assert_start_sent(pm);
  assert_no_stop_sent(pm);

  pm->prev_patch();
  assert_start_sent(pm);
  assert_stop_sent(pm);

  pm->stop();
}

TEST_CASE("next song start and stop messages", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_file();
  pm->start();

  pm->next_song();
  assert_no_start_sent(pm);
  assert_no_stop_sent(pm);

  pm->next_patch();
  assert_start_sent(pm);
  assert_no_stop_sent(pm);

  pm->prev_song();
  assert_start_sent(pm);
  assert_stop_sent(pm);

  pm->stop();
}
