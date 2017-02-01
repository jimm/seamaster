#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <err.h>
#include "portmidi.h"
#include "patchmaster.h"
#include "loader.h"
#include "debug.h"

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

  pm = pm;
  song = 0;
  patch = 0;
  song_list = 0;
  while (fgets(line, BUFSIZ, fp) != 0) {
    strip_newline(line);
    parse_line(line);
  }
  fclose(fp);
  pm.debug();
  return 0;
}

void Loader::parse_line(char *line) {
  int start = strspn(line, whitespace);
  if (line[start] == 0 || line[start] == '#') // whitespace only or comment
    return;

  line += start;                // strip leading whitespace
  int ch = line[0];
  switch (ch) {
  case 'i': case 'o':
    load_instrument(line, ch);
    break;
  case 'm':
    if (line[1] == 'e')
      load_message(line);
    else
      load_map(line);
    break;
  case 't':
    load_trigger(line);
    break;
  case 's':
    switch (line[1]) {
    case 'o':                   // song
      load_song(line);
      break;
    case 'e':                   // set
      load_song_list(line);
      break;
    }
    break;
  case 'p':
    switch (line[1]) {
    case 'a':                   // patch
      load_patch(line);
      break;
    case 'c': case 'r':         // program change
      load_prog(line);
      break;
    }
    break;
  case 'c':
    load_connection(line);
    break;
  case 'x':
    load_xpose(line);
    break;
  case 'f':
    load_filter(line);
    break;
  case 'n':
    load_notes();
    break;
  case 'l':
    load_song_list(line);
    break;
  // TODO patch start and stop messages
  }
}

int Loader::load_instrument(char *line, int type) {
  List<char *> *args = comma_sep_args(line);
  PmDeviceID devid = find_device((char *)args->at(0), type);

  if (devid == pmNoDevice && !pm.testing)
    return 1;

  char *sym = (char *)args->at(1);
  char *name = (char *)args->at(2);

  switch (type) {
  case 'i':
    pm.inputs << new Input(sym, name, devid);
    break;
  case 'o':
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
  char *name = skip_first_word(line);
  Song *s = new Song(name);
  pm.all_songs->songs << s;
  song = s;
  patch = 0;
  conn = 0;
  return 0;
}

int Loader::load_notes() {
  char line[BUFSIZ];
  while (fgets(line, BUFSIZ, fp) != 0 && strncmp(line, "end", 3) != 0)
    song->append_notes(line);
  return 0;
}

int Loader::load_patch(char *line) {
  char *name = skip_first_word(line);
  Patch *p = new Patch(name);
  song->patches << p;
  patch = p;
  conn = 0;
  return 0;
}

int Loader::load_connection(char *line) {
  List<char *> *args = comma_sep_args(line);
  Input *in = (Input *)find_by_sym(reinterpret_cast<List<Instrument *> &>(pm.inputs), (char *)args->at(0));
  int in_chan = chan_from_word((char *)args->at(1));
  Output *out = (Output *)find_by_sym(reinterpret_cast<List<Instrument *> &>(pm.outputs), (char *)args->at(2));
  int out_chan = chan_from_word((char *)args->at(3));

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
  List<char *> *args = comma_sep_args(line);
  conn->prog.bank_msb = atoi((char *)args->at(0));
  conn->prog.bank_lsb = atoi((char *)args->at(1));
  return 0;
}

int Loader::load_xpose(char *line) {
  char *amount = skip_first_word(line);
  conn->xpose = atoi(amount);
  return 0;
}

int Loader::load_filter(char *line) {
  int controller = atoi(skip_first_word(line));
  conn->cc_maps[controller] = -1;
  return 0;
}

int Loader::load_map(char *line) {
  List<char *> *args = comma_sep_args(line);
  conn->cc_maps[atoi((char *)args->at(0))] = atoi((char *)args->at(1));
  delete args;
  return 0;
}

int Loader::load_song_list(char *line) {
  char *name = skip_first_word(line);
  SongList *sl = new SongList(name);
  pm.song_lists << sl;

  char song_name[BUFSIZ];
  while (fgets(song_name, BUFSIZ, fp) != 0 && strncmp(song_name, "end\n", 4) != 0) {
    strip_newline(song_name);
    Song *s = find_song(pm.all_songs->songs, song_name);
    if (s == 0)
      fprintf(stderr, "error in set: can not find song named \"%s\"\n", song_name);
    else
      sl->songs << find_song(pm.all_songs->songs, song_name);
  }
  song_list = sl;

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
List<char *> *Loader::comma_sep_args(char *line) {
  List<char *> *l = new List<char *>();
  char *args_start = skip_first_word(line);

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

PmDeviceID Loader::find_device(char *name, char in_or_out) {
  int num_devices = Pm_CountDevices();
  for (int i = 0; i < num_devices; ++i) {
    const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
    if (in_or_out == 'i' && info->input && strcmp(name, info->name) == 0)
      return i;
    if (in_or_out == 'o' && info->output && strcmp(name, info->name) == 0)
      return i;
  }
  return pmNoDevice;
}

Instrument *Loader::find_by_sym(List<Instrument *> &list, char *name) {
  for (int i = 0; i < list.length(); ++i) {
    Instrument *inst = (Instrument *)list[i];
    if (inst->sym == name)
      return inst;
  }
  return 0;
}

Song *Loader::find_song(List<Song *> &list, char *name) {
  for (int i = 0; i < list.length(); ++i) {
    Song *s = (Song *)list[i];
    if (s->name == name)
      return s;
  }
  return 0;
}
