#include <stdarg.h>
#include <stdio.h>
#include "debug.h"

#ifdef DEBUG

#define DEBUG_FILE "/tmp/seamaster_debug.log"

void debug(const char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  FILE *fp = fopen(DEBUG_FILE, "a");
  vfprintf(fp == 0 ? stderr : fp, fmt, ap);
  va_end(ap);
  if (fp)
    fclose(fp);
}

#else

void debug(const char *fmt, ...) {
}

#endif
