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
list *comma_sep_args(char *);
int chan_from_word(char *);
void strip_newline(char *);

PmDeviceID find_device(char *, char);
Instrument *find_by_sym(list *, char *);
Song *find_song(list *, char *);

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
  list *args = comma_sep_args(line);
  PmDeviceID devid = find_device((char *)list_first(args), type);

  if (devid == pmNoDevice && !c->pm->testing)
    return 1;

  char *sym = (char *)list_at(args, 1);
  char *name = (char *)list_at(args, 2);

  switch (type) {
  case 'i':
    list_append(c->pm->inputs, new Input(sym, name, devid));
    break;
  case 'o':
    list_append(c->pm->outputs, new Output(sym, name, devid));
    break;
  }
  list_free(args, 0);
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
  list_append(c->pm->all_songs->songs, s);
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
  list_append(c->song->patches, p);
  c->patch = p;
  c->conn = 0;
  return 0;
}

int load_connection(context *c, char *line) {
  list *args = comma_sep_args(line);
  Input *in = (Input *)find_by_sym(c->pm->inputs, (char *)list_first(args));
  int in_chan = chan_from_word((char *)list_at(args, 1));
  Output *out = (Output *)find_by_sym(c->pm->outputs, (char *)list_at(args, 2));
  int out_chan = chan_from_word((char *)list_at(args, 3));

  Connection *conn = new Connection(in, in_chan, out, out_chan);
  list_append(c->patch->connections, conn);
  c->conn = conn;

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
  list_append(c->pm->song_lists, sl);

  char song_name[BUFSIZ];
  while (fgets(song_name, BUFSIZ, c->fp) != 0 && strncmp(song_name, "end\n", 4) != 0) {
    strip_newline(song_name);
    Song *s = find_song(c->pm->all_songs->songs, song_name);
    if (s == 0)
      fprintf(stderr, "error in set: can not find song named \"%s\"\n", song_name);
    else
      list_append(sl->songs, find_song(c->pm->all_songs->songs, song_name));
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
  list *args = comma_sep_args(line);
  c->conn->cc_maps[atoi((char *)list_at(args, 0))] = atoi((char *)list_at(args, 1));
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
list *comma_sep_args(char *line) {
  list *l = list_new();
  char *args_start = skip_first_word(line);

  char *word;
  for (word = strtok(args_start, ","); word != 0; word = strtok(0, ",")) {
    word += strspn(word, whitespace);
    list_append(l, word);
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

Instrument *find_by_sym(list *list, char *name) {
  for (int i = 0; i < list_length(list); ++i) {
    Instrument *inst = (Instrument *)list_at(list, i);
    if (strcmp(name, inst->sym) == 0)
      return inst;
  }
  return 0;
}

Song *find_song(list *list, char *name) {
  for (int i = 0; i < list_length(list); ++i) {
    Song *s = (Song *)list_at(list, i);
    if (strcmp(name, s->name) == 0)
      return s;
  }
  return 0;
}
