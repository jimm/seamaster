#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

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
int note_name_to_num(char *str) {
  char ch = str[0];

  if (isdigit(ch))
    return atoi(str);

  ch = tolower(ch);
  if (ch < 'a' || ch > 'g')
    return 0;

  int from_c = NOTE_OFFSETS[ch - 'a'];
  char *num_start = str+1;
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
