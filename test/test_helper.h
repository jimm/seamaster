#ifndef TEST_HELPER_H
#define TEST_HELPER_H

#include <portmidi.h>
#include "../src/consts.h"
#include "../src/connection.h"
#include "../src/patchmaster.h"
#include "../src/input.h"
#include "../src/output.h"
#include "../src/cursor.h"

PatchMaster *load_test_file();
PatchMaster *load_test_file(const char *path);
Connection *create_conn();

#endif /* TEST_HELPER_H */
