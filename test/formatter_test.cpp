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
  tassert(note_name_to_num("c-1") == 0, 0);
  tassert(note_name_to_num("C#-1") == 1, 0);
  tassert(note_name_to_num("e4") == 64, 0);
  tassert(note_name_to_num("e3") == 52, 0);
  tassert(note_name_to_num("fs3") == 54, 0);
  tassert(note_name_to_num("f#3") == 54, 0);
  tassert(note_name_to_num("ef3") == 51, 0);
  tassert(note_name_to_num("eb3") == 51, 0);
  tassert(note_name_to_num("g9") == 127, 0);
  tassert(note_name_to_num("G9") == 127, 0);
}

void test_note_name_to_num_given_num() {
  tassert(note_name_to_num("0") == 0, 0);
  tassert(note_name_to_num("42") == 42, 0);
}

void test_formatter() {
  test_run(test_note_num_to_name);
  test_run(test_note_name_to_num);
  test_run(test_note_name_to_num_given_num);
}
