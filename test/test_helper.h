#ifndef TEST_HELPER_H
#define TEST_HELPER_H

#include "seamaster_test.h"
#include "../src/connection.h"
#include "../src/patchmaster.h"
#include "../src/input.h"
#include "../src/output.h"
#include "../src/cursor.h"

PatchMaster *create_pm();
Connection *create_conn();

#endif /* TEST_HELPER_H */
