#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <err.h>
#include "portmidi.h"
#include "patchmaster.h"
#include "load.h"
#include "debug.h"

typedef struct context {
  FILE *fp;
  PatchMaster *pm;
  Song *song;
  Patch *patch;
  Connection *conn;
  SongList *song_list;
} context;

const char * const whitespace = " \t";

void parse_line(context *, char *);
char *skip_first_word(char *);
List *comma_sep_args(char *);
int chan_from_word(char *);
void strip_newline(char *);

PmDeviceID find_device(char *, char);
Instrument *find_by_sym(List &, char *);
Song *find_song(List &, char *);

int load_instrument(context *, char *, int);
int load_message(context *, char *);
int load_trigger(context *, char *);
int load_song(context *, char *);
int load_notes(context *);
int load_patch(context *, char *);
int load_connection(context *, char *);
int load_xpose(context *, char *);
int load_filter(context *, char *);
int load_map(context *, char *);
int load_song_list(context *, char *);

int load(PatchMaster *pm, const char *path) {
  int rc;
  context ctxt;
  char line[BUFSIZ];

  ctxt.fp = fopen(path, "r");
  if (ctxt.fp == 0) {
    err(errno, "%s", path);
    return 1;
  }

  ctxt.pm = pm;
  ctxt.song = 0;
  ctxt.patch = 0;
  ctxt.song_list = 0;
  while (fgets(line, BUFSIZ, ctxt.fp) != 0) {
    strip_newline(line);
    parse_line(&ctxt, line);
  }
  fclose(ctxt.fp);
  pm->debug();
  return 0;
}

void parse_line(context *c, char *line) {
  int start = strspn(line, whitespace);
  if (line[start] == 0 || line[start] == '#') /* whitespace only or comment */
    return;

  line += start;                /* strip leading whitespace */
  int ch = line[0];
  switch (ch) {
  case 'i': case 'o':
    load_instrument(c, line, ch);
    break;
  case 'm':
    if (line[1] == 'e')
      load_message(c, line);
    else
      load_map(c, line);
    break;
  case 't':
    load_trigger(c, line);
    break;
  case 's':
    switch (line[1]) {
    case 'o':                   /* song */
      load_song(c, line);
      break;
    case 'e':                   /* set */
      load_song_list(c, line);
      break;
    }
    break;
  case 'p':
    load_patch(c, line);
    break;
  case 'c':
    load_connection(c, line);
    break;
  case 'x':
    load_xpose(c, line);
    break;
  case 'f':
    load_filter(c, line);
    break;
  case 'n':
    load_notes(c);
    break;
  case 'l':
    load_song_list(c, line);
    break;
  // TODO patch start and stop messages
  }
}

int load_instrument(context *c, char *line, int type) {
  List *args = comma_sep_args(line);
  PmDeviceID devid = find_device((char *)args->at(0), type);

  if (devid == pmNoDevice && !c->pm->testing)
    return 1;

  char *sym = (char *)args->at(1);
  char *name = (char *)args->at(2);

  switch (type) {
  case 'i':
    c->pm->inputs << new Input(sym, name, devid);
    break;
  case 'o':
    c->pm->outputs << new Output(sym, name, devid);
    break;
  }

  delete args;
  return 0;
}

int load_message(context *c, char *line) {
  // TODO
  return 0;
}

int load_trigger(context *c, char *line) {
  // TODO
  return 0;
}

int load_song(context *c, char *line) {
  char *name = skip_first_word(line);
  Song *s = new Song(name);
  c->pm->all_songs->songs << s;
  c->song = s;
  c->patch = 0;
  c->conn = 0;
  return 0;
}

int load_notes(context *c) {
  char line[BUFSIZ];
  while (fgets(line, BUFSIZ, c->fp) != 0 && strncmp(line, "end", 3) != 0)
    c->song->append_notes(line);
  return 0;
}

int load_patch(context *c, char *line) {
  char *name = skip_first_word(line);
  Patch *p = new Patch(name);
  c->song->patches << p;
  c->patch = p;
  c->conn = 0;
  return 0;
}

int load_connection(context *c, char *line) {
  List *args = comma_sep_args(line);
  Input *in = (Input *)find_by_sym(c->pm->inputs, (char *)args->at(0));
  int in_chan = chan_from_word((char *)args->at(1));
  Output *out = (Output *)find_by_sym(c->pm->outputs, (char *)args->at(2));
  int out_chan = chan_from_word((char *)args->at(3));

  Connection *conn = new Connection(in, in_chan, out, out_chan);
  c->patch->connections << conn;
  c->conn = conn;

  delete args;
  return 0;
}

int load_xpose(context *c, char *line) {
  char *amount = skip_first_word(line);
  c->conn->xpose = atoi(amount);
  return 0;
}

int load_song_list(context *c, char *line) {
  char *name = skip_first_word(line);
  SongList *sl = new SongList(name);
  c->pm->song_lists << sl;

  char song_name[BUFSIZ];
  while (fgets(song_name, BUFSIZ, c->fp) != 0 && strncmp(song_name, "end\n", 4) != 0) {
    strip_newline(song_name);
    Song *s = find_song(c->pm->all_songs->songs, song_name);
    if (s == 0)
      fprintf(stderr, "error in set: can not find song named \"%s\"\n", song_name);
    else
      sl->songs << find_song(c->pm->all_songs->songs, song_name);
  }
  c->song_list = sl;

  return 0;
}

int load_filter(context *c, char *line) {
  int controller = atoi(skip_first_word(line));
  c->conn->cc_maps[controller] = -1;
  return 0;
}

int load_map(context *c, char *line) {
  List *args = comma_sep_args(line);
  c->conn->cc_maps[atoi((char *)args->at(0))] = atoi((char *)args->at(1));
  delete args;
  return 0;
}

void strip_newline(char *line) {
  int len = strlen(line);
  if (line[len-1] == '\n')
    line[len-1] = 0;
}

char *skip_first_word(char *line) {
  char *after_leading_spaces = line + strspn(line, whitespace);
  char *after_word = after_leading_spaces + strcspn(line, whitespace);
  return after_word + strspn(after_word, whitespace);
}

/*
 * Skips first word on line, splits rest of line on commas, and returns the
 * list as a list of strings. The contents should NOT be freed, since they
 * are a destructive mutation of `line`.
 */
List *comma_sep_args(char *line) {
  List *l = new List();
  char *args_start = skip_first_word(line);

  char *word;
  for (word = strtok(args_start, ","); word != 0; word = strtok(0, ",")) {
    word += strspn(word, whitespace);
    l->append(word);
  }

  return l;
}

int chan_from_word(char *word) {
  return strcmp(word, "all") == 0 ? -1 : atoi(word) - 1;
}

PmDeviceID find_device(char *name, char in_or_out) {
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

Instrument *find_by_sym(List &list, char *name) {
  for (int i = 0; i < list.length(); ++i) {
    Instrument *inst = (Instrument *)list[i];
    if (inst->sym == name)
      return inst;
  }
  return 0;
}

Song *find_song(List &list, char *name) {
  for (int i = 0; i < list.length(); ++i) {
    Song *s = (Song *)list[i];
    if (s->name == name)
      return s;
  }
  return 0;
}
