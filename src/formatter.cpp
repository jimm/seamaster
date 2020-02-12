#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "consts.h"
#include "connection.h"

static const char * NOTE_NAMES[] = {
  "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
};
static const int NOTE_OFFSETS[] = {
  9, 11, 0, 2, 4, 5, 7
};

void note_num_to_name(int num, char *buf) {
  int oct = (num / 12) - 1;
  const char *note = NOTE_NAMES[num % 12];
  sprintf(buf, "%s%d", note, oct);
}

// str may point to an integer string like "64" as well
int note_name_to_num(const char *str) {
  char ch = str[0];

  if (isdigit(ch))
    return atoi(str);

  ch = tolower(ch);
  if (ch < 'a' || ch > 'g')
    return 0;

  int from_c = NOTE_OFFSETS[ch - 'a'];
  const char *num_start = str+1;
  int accidental = 0;
  switch (tolower(*num_start)) {
  case 's': case '#':
    accidental = 1;
    ++num_start;
    break;
  case 'f': case 'b':
    accidental = -1;
    ++num_start;
    break;
  }

  int octave = (atoi(num_start) + 1) * 12;
  return octave + from_c + accidental;
}

string byte_to_hex(unsigned char byte) {
  char buf[3];
  snprintf(buf, 3, "%02x", byte);
  return string((const char *)buf);
}

void format_program(program prog, char *buf) {
  int has_msb = prog.bank_msb != UNDEFINED;
  int has_lsb = prog.bank_lsb != UNDEFINED;
  int has_bank = has_msb || has_lsb;

  sprintf(buf, " %c", has_bank ? '[' : ' ');
  buf += 2;

  if (has_msb)
    sprintf(buf, "%3d", prog.bank_msb);
  else
    strcat(buf, "   ");
  buf += 3;

  sprintf(buf, "%c ", has_bank ? ',' : ' ');
  buf += 2;

  if (has_lsb)
    sprintf(buf, "%3d", prog.bank_lsb);
  else
    strcat(buf, "   ");
  buf += 3;

  sprintf(buf, "%c ", has_bank ? ']' : ' ');
  buf += 2;

  if (prog.prog != UNDEFINED)
    sprintf(buf, " %3d", prog.prog);
  else
    strcat(buf, "    ");
}

void format_program_no_spaces(program prog, char *buf) {
  int has_msb = prog.bank_msb != UNDEFINED;
  int has_lsb = prog.bank_lsb != UNDEFINED;
  int has_bank = has_msb || has_lsb;

  if (has_bank)
    *buf++ = '[';

  if (has_msb) {
    sprintf(buf, "%3d", prog.bank_msb);
    buf += 3;
  }

  if (has_bank)
    *buf++ = ',';

  if (has_lsb) {
    sprintf(buf, "%3d", prog.bank_lsb);
    buf += 3;
  }

  if (has_bank)
    *buf++ = ']';

  if (prog.prog != UNDEFINED) {
    sprintf(buf, "%3d", prog.prog);
    buf += 3;
  }

  *buf = 0;
}

void format_controllers(Connection *conn, char *buf) {
  int first = true;

  strcat(buf, " ");
  buf += 1;
  for (int i = 0; i < 128; ++i) {
    Controller *cc = conn->cc_maps[i];
    if (cc == nullptr || !cc->will_modify())
      continue;

    if (first) first = false; else { strcat(buf, ", "); buf += 2; }
    sprintf(buf, "%d", cc->cc_num);
    buf += strlen(buf);

    if (cc->filtered) {
      sprintf(buf, "x");
      buf += 1;
      continue;
    }

    if (cc->cc_num != cc->translated_cc_num) {
      sprintf(buf, "->%d", cc->translated_cc_num);
      buf += strlen(buf);
    }

    if (cc->min != 0 || cc->max != 127) {
      sprintf(buf, " [%d, %d]", cc->min, cc->max);
      buf += strlen(buf);
    }
  }
  *buf = 0;
}
