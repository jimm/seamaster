#include "test_helper.h"
#include "../src/formatter.h"
#include "formatter_test.h"

void test_note_num_to_name() {
  char buf[BUFSIZ];

  note_num_to_name(0, buf);
  tassert(strcmp("C-1", buf) == 0, 0);
  note_num_to_name(1, buf);
  tassert(strcmp("C#-1", buf) == 0, 0);
  note_num_to_name(64, buf);
  tassert(strcmp("E4", buf) == 0, 0);
  note_num_to_name(52, buf);
  tassert(strcmp("E3", buf) == 0, 0);
  note_num_to_name(54, buf);
  tassert(strcmp("F#3", buf) == 0, 0);
  note_num_to_name(51, buf);
  tassert(strcmp("D#3", buf) == 0, 0);
  note_num_to_name(127, buf);
  tassert(strcmp("G9", buf) == 0, 0);
}

void test_note_name_to_num() {
  tassert(note_name_to_num((char *)"c-1") == 0, 0);
  tassert(note_name_to_num((char *)"C#-1") == 1, 0);
  tassert(note_name_to_num((char *)"e4") == 64, 0);
  tassert(note_name_to_num((char *)"e3") == 52, 0);
  tassert(note_name_to_num((char *)"fs3") == 54, 0);
  tassert(note_name_to_num((char *)"f#3") == 54, 0);
  tassert(note_name_to_num((char *)"ef3") == 51, 0);
  tassert(note_name_to_num((char *)"eb3") == 51, 0);
  tassert(note_name_to_num((char *)"g9") == 127, 0);
  tassert(note_name_to_num((char *)"G9") == 127, 0);
}

void test_formatter() {
  test_run(test_note_num_to_name);
  test_run(test_note_name_to_num);
}
