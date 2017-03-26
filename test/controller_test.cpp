#include "test_helper.h"
#include "controller_test.h"

void test_cc_out_chan() {
  Controller cc;
  cc.cc_num = 7;
  tassert(cc.process(Pm_Message(CONTROLLER, 7, 127), 3) == Pm_Message(CONTROLLER + 3, 7, 127),
          "bad channel mapping");
}

void test_cc_filter() {
  Controller cc;
  cc.cc_num = 7;
  cc.filtered = true;
  tassert(cc.process(Pm_Message(CONTROLLER, 7, 127), 0) == -1, "should have filtered");
}

void test_cc_map() {
  Controller cc;
  cc.cc_num = 7;
  cc.translated_cc_num = 10;
  tassert(cc.process(Pm_Message(CONTROLLER, 7, 127), 0) == Pm_Message(CONTROLLER, 10, 127),
          "should have filtered");
}

void test_cc_limit() {
  Controller cc;
  cc.cc_num = 7;
  cc.min = 1;
  cc.max = 120;

  tassert(Pm_MessageData2(cc.process(Pm_Message(CONTROLLER, 7, 0), 0)) == 1, 0);
  tassert(Pm_MessageData2(cc.process(Pm_Message(CONTROLLER, 7, 1), 0)) == 1, 0);
  tassert(Pm_MessageData2(cc.process(Pm_Message(CONTROLLER, 7, 64), 0)) == 64, 0);
  tassert(Pm_MessageData2(cc.process(Pm_Message(CONTROLLER, 7, 120), 0)) == 120, 0);
  tassert(Pm_MessageData2(cc.process(Pm_Message(CONTROLLER, 7, 121), 0)) == 120, 0);
  tassert(Pm_MessageData2(cc.process(Pm_Message(CONTROLLER, 7, 127), 0)) == 120, 0);
}

void test_controller() {
  test_run(test_cc_out_chan);
  test_run(test_cc_filter);
  test_run(test_cc_map);
  test_run(test_cc_limit);
}
