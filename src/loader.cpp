#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <err.h>
#include "portmidi.h"
#include "patchmaster.h"
#include "loader.h"
#include "formatter.h"
#include "debug.h"
  
#define INSTRUMENT_INPUT 0
#define INSTRUMENT_OUTPUT 1

const char * const whitespace = " \t";

Loader::Loader(PatchMaster &pmaster)
  : pm(pmaster)
{
}

Loader::~Loader() {
}

int Loader::load(const char *path) {
  int rc;
  char line[BUFSIZ];

  fp = fopen(path, "r");
  if (fp == 0) {
    err(errno, "%s", path);
    return 1;
  }

  clear();
  while (fgets(line, BUFSIZ, fp) != 0) {
    strip_newline(line);
    parse_line(line);
  }
  fclose(fp);
  pm.debug();
  return 0;
}

void Loader::clear() {
  section = IGNORE;
  notes_state = OUTSIDE;
  song = 0;
  patch = 0;
  conn = 0;
  song_list = 0;
}

void Loader::enter_section(Section sec) {
  clear();
  section = sec;
}

void Loader::parse_line(char *line) {
  if (notes_state == OUTSIDE) {
    int start = strspn(line, whitespace);
    if (line[start] == 0 || strncmp(line + start, "# ", 2) == 0) // whitespace only or comment
      return;

    line += start;              // strip leading whitespace
  }

  if (strncmp(line, "* Instruments", 13) == 0) {
    enter_section(INSTRUMENTS);
    return;
  }
  if (strncmp(line, "* Songs", 7) == 0) {
    enter_section(SONGS);
    return;
  }
  if (strncmp(line, "* Set Lists", 11) == 0) {
    enter_section(SET_LISTS);
    return;
  }
  if (strncmp(line, "* ", 2) == 0) {
    enter_section(IGNORE);
    return;
  }

  switch (section) {
  case INSTRUMENTS:
    parse_instrument_line(line);
    break;
  case MESSAGES:
    parse_message_line(line);
    break;
  case TRIGGERS:
    parse_trigger_line(line);
    break;
  case SONGS:
    parse_song_line(line);
    break;
  case SET_LISTS:
    parse_set_list_line(line);
    break;
  default:
    break;
  }
}

void Loader::parse_instrument_line(char *line) {
  if (strncmp("- input ", line, 8) == 0)
    load_instrument(line + 2, INSTRUMENT_INPUT);
  else if (strncmp("- output ", line, 9) == 0)
    load_instrument(line + 2, INSTRUMENT_OUTPUT);
}

void Loader::parse_message_line(char *line) {
  // TODO
}

void Loader::parse_trigger_line(char *line) {
  // TODO
}

void Loader::parse_song_line(char *line) {
  if (strncmp("**** ", line, 5) == 0)
    load_connection(line + 5);
  else if (strncmp("*** ", line, 4) == 0)
    load_patch(line + 4);
  else if (strncmp("** ", line, 3) == 0)
    load_song(line + 3);
  else if (notes_state != OUTSIDE)
    load_notes_line(line);
  else if (strncmp("- ", line, 2) == 0 && conn != 0) {
    line += 2;
    char ch = line[0];
    switch (ch) {
    case 'b':
      load_bank(line);
      break;
    case 'p':
      load_prog(line);
      break;
    case 'x':
      load_xpose(line);
      break;
    case 'z':
      load_zone(line);
      break;
    case 'f':
      load_filter(line);
      break;
    case 'm':
      load_map(line);
      break;
    }
  }
}

void Loader::parse_set_list_line(char *line) {
  if (strncmp("** ", line, 3) == 0)
    load_song_list(line + 3);
  else if (strncmp("- ", line, 2) == 0)
    load_song_list_song(line + 2);
}

int Loader::load_instrument(char *line, int type) {
  List<char *> *args = comma_sep_args(skip_first_word(line), false);
  PmDeviceID devid = find_device(args->at(0), type);

  if (devid == pmNoDevice && !pm.testing)
    return 1;

  char *sym = args->at(1);
  char *name = args->at(2);

  switch (type) {
  case INSTRUMENT_INPUT:
    pm.inputs << new Input(sym, name, devid);
    break;
  case INSTRUMENT_OUTPUT:
    pm.outputs << new Output(sym, name, devid);
    break;
  }

  delete args;
  return 0;
}

int Loader::load_message(char *line) {
  // TODO
  return 0;
}

int Loader::load_trigger(char *line) {
  // TODO
  return 0;
}

int Loader::load_song(char *line) {
  Song *s = new Song(line);
  pm.all_songs->songs << s;
  song = s;
  patch = 0;
  conn = 0;
  notes_state = SKIPPING_BLANK_LINES;
  return 0;
}

int Loader::load_notes_line(char *line) {
  if (notes_state == SKIPPING_BLANK_LINES && strlen(line) == 0)
    return 0;

  notes_state = COLLECTING;
  song->append_notes(line);
  return 0;
}

void Loader::stop_collecting_notes() {
  if (song != 0) {              // remove trailing blank lines
    while (song->notes.length() > 0 && strlen(song->notes.last()) == 0)
      song->notes.remove_at(song->notes.length()-1);
  }
  notes_state = OUTSIDE;
}

int Loader::load_patch(char *line) {
  Patch *p = new Patch(line);
  song->patches << p;
  patch = p;
  conn = 0;
  stop_collecting_notes();
  return 0;
}

int Loader::load_connection(char *line) {
  List<char *> *args = comma_sep_args(line, false);
  Input *in = (Input *)find_by_sym(reinterpret_cast<List<Instrument *> &>(pm.inputs), (char *)args->at(0));
  int in_chan = chan_from_word(args->at(1));
  Output *out = (Output *)find_by_sym(reinterpret_cast<List<Instrument *> &>(pm.outputs), (char *)args->at(2));
  int out_chan = chan_from_word(args->at(3));

  conn = new Connection(in, in_chan, out, out_chan);
  patch->connections << conn;

  delete args;
  return 0;
}

int Loader::load_prog(char *line) {
  char *prog_chg = skip_first_word(line);
  conn->prog.prog = atoi(prog_chg);
  return 0;
}

int Loader::load_bank(char *line) {
  List<char *> *args = comma_sep_args(line, true);
  conn->prog.bank_msb = atoi(args->at(0));
  conn->prog.bank_lsb = atoi(args->at(1));
  return 0;
}

int Loader::load_xpose(char *line) {
  char *amount = skip_first_word(line);
  conn->xpose = atoi(amount);
  return 0;
}

int Loader::load_zone(char *line) {
  List<char *> *args = comma_sep_args(line, true);
  conn->zone.low = note_name_to_num(args->at(0));
  conn->zone.high = note_name_to_num(args->at(1));
  return 0;
}

int Loader::load_filter(char *line) {
  int controller = atoi(skip_first_word(line));
  conn->cc_maps[controller] = -1;
  return 0;
}

int Loader::load_map(char *line) {
  List<char *> *args = comma_sep_args(line, true);
  conn->cc_maps[atoi(args->at(0))] = atoi(args->at(1));
  delete args;
  return 0;
}

int Loader::load_song_list(char *line) {
  song_list = new SongList(line);
  pm.song_lists << song_list;
  return 0;
}

int Loader::load_song_list_song(char *line) {
  Song *s = find_song(pm.all_songs->songs, line);
  if (s == 0) {
    fprintf(stderr, "error in set: can not find song named \"%s\"\n", line);
    return 1;
  }

  song_list->songs << find_song(pm.all_songs->songs, line);
  return 0;
}

void Loader::strip_newline(char *line) {
  int len = strlen(line);
  if (line[len-1] == '\n')
    line[len-1] = 0;
}

char *Loader::skip_first_word(char *line) {
  char *after_leading_spaces = line + strspn(line, whitespace);
  char *after_word = after_leading_spaces + strcspn(line, whitespace);
  return after_word + strspn(after_word, whitespace);
}

/*
 * Skips first word on line, splits rest of line on commas, and returns the
 * list as a list of strings. The contents should NOT be freed, since they
 * are a destructive mutation of `line`.
 */
List<char *> *Loader::comma_sep_args(char *line, bool skip_word) {
  List<char *> *l = new List<char *>();
  char *args_start = skip_word ? skip_first_word(line) : line;

  char *word;
  for (word = strtok(args_start, ","); word != 0; word = strtok(0, ",")) {
    word += strspn(word, whitespace);
    l->append(word);
  }

  return l;
}

int Loader::chan_from_word(char *word) {
  return strcmp(word, "all") == 0 ? -1 : atoi(word) - 1;
}

PmDeviceID Loader::find_device(char *name, int device_type) {
  if (pm.testing)
    return pmNoDevice;

  int num_devices = Pm_CountDevices();
  for (int i = 0; i < num_devices; ++i) {
    const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
    if (device_type == INSTRUMENT_INPUT && info->input && strcmp(name, info->name) == 0)
      return i;
    if (device_type == INSTRUMENT_OUTPUT && info->output && strcmp(name, info->name) == 0)
      return i;
  }
  return pmNoDevice;
}

Instrument *Loader::find_by_sym(List<Instrument *> &list, char *name) {
  for (int i = 0; i < list.length(); ++i)
    if (list[i]->sym == name)
      return list[i];
  return 0;
}

Song *Loader::find_song(List<Song *> &list, char *name) {
  for (int i = 0; i < list.length(); ++i)
    if (list[i]->name == name)
      return list[i];
  return 0;
}
