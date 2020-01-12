#include "../src/patchmaster.h"
#include "test_helper.h"
#include "loader_test.h"

void assert_no_start_sent(PatchMaster *pm) {
  for (auto& out : pm->outputs) {
    for (int i = 0; i < out->num_io_messages; ++i)
      if (out->io_messages[i] == Pm_Message(0xb0, 7, 127))
        tassert(false, "start message seen, but was not expected");
  }
  tassert(true, 0);
}

void assert_no_stop_sent(PatchMaster *pm) {
  for (auto& out : pm->outputs) {
    for (int i = 0; i < out->num_io_messages; ++i)
      if (out->io_messages[i] == Pm_Message(0xb2, 7, 127))
        tassert(false, "stop message seen, but was not expected");
  }
  tassert(true, 0);
}

void assert_start_sent(PatchMaster *pm) {
  for (auto& out : pm->outputs) {
    for (int i = 0; i < out->num_io_messages; ++i)
      if (out->io_messages[i] == Pm_Message(0xb0, 7, 127))
        return;
  }
  tassert(false, "start message not sent, but was expected");
}

void assert_stop_sent(PatchMaster *pm) {
  Output *out = pm->outputs[0];
  for (int i = 0; i < out->num_io_messages; ++i)
    if (out->io_messages[i] == Pm_Message(0xb2, 7, 127))
      return;
  tassert(false, "stop message not sent, but was expected");
}

void test_patchmaster_next_patch_start_and_stop_messages() {
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

void test_patchmaster_next_song_start_and_stop_messages() {
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

void test_patchmaster() {
  test_patchmaster_next_patch_start_and_stop_messages();
  test_patchmaster_next_song_start_and_stop_messages();
}
