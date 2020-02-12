#include "catch.hpp"
#include "test_helper.h"

#define CATCH_CATEGORY "[controller]"

TEST_CASE("will modify", CATCH_CATEGORY) {
  Controller cc(7);

  REQUIRE(cc.will_modify() == false);

  cc.filtered = true;
  REQUIRE(cc.will_modify() == true);
  cc.filtered = false;

  cc.translated_cc_num = 8;
  REQUIRE(cc.will_modify() == true);
  cc.translated_cc_num = 7;

  cc.min = 1;
  REQUIRE(cc.will_modify() == true);
  cc.min = 0;

  cc.max = 126;
  REQUIRE(cc.will_modify() == true);
  cc.max = 127;
}

TEST_CASE("out chan", CATCH_CATEGORY) {
  Controller cc(7);
  REQUIRE(cc.process(Pm_Message(CONTROLLER, 7, 127), 3) == Pm_Message(CONTROLLER + 3, 7, 127));
}

TEST_CASE("filter", CATCH_CATEGORY) {
  Controller cc(7);
  cc.filtered = true;
  REQUIRE(cc.process(Pm_Message(CONTROLLER, 7, 127), 0) == CONTROLLER_BLOCK);
}

TEST_CASE("map", CATCH_CATEGORY) {
  Controller cc(7);
  cc.translated_cc_num = 10;
  REQUIRE(cc.process(Pm_Message(CONTROLLER, 7, 127), 0) == Pm_Message(CONTROLLER, 10, 127));
}

TEST_CASE("limit", CATCH_CATEGORY) {
  Controller cc(7);
  cc.min = 1;
  cc.max = 120;

  REQUIRE(Pm_MessageData2(cc.process(Pm_Message(CONTROLLER, 7, 0), 0)) == 1);
  REQUIRE(Pm_MessageData2(cc.process(Pm_Message(CONTROLLER, 7, 1), 0)) == 1);
  REQUIRE(Pm_MessageData2(cc.process(Pm_Message(CONTROLLER, 7, 64), 0)) == 64);
  REQUIRE(Pm_MessageData2(cc.process(Pm_Message(CONTROLLER, 7, 120), 0)) == 120);
  REQUIRE(Pm_MessageData2(cc.process(Pm_Message(CONTROLLER, 7, 121), 0)) == 120);
  REQUIRE(Pm_MessageData2(cc.process(Pm_Message(CONTROLLER, 7, 127), 0)) == 120);
}
