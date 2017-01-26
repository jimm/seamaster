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
  patchmaster *pm;
  song *song;
  patch *patch;
  connection *conn;
  song_list *song_list;
} context;

const char * const whitespace = " \t";

void parse_line(context *, char *);
char *skip_first_word(char *);
list *comma_sep_args(char *);
int chan_from_word(char *);
void strip_newline(char *);

PmDeviceID find_device(char *, char);
input *find_by_sym(list *, char *);
song *find_song(list *, char *);

int load_instrument(context *, char *, int);
int load_message(context *, char *);
int load_trigger(context *, char *);
int load_song(context *, char *);
int load_notes(context *);
int load_patch(context *, char *);
int load_connection(context *, char *);
int load_xpose(context *, char *);
int load_song_list(context *, char *);

int load(patchmaster *pm, const char *path) {
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
  patchmaster_debug(pm);
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
    load_message(c, line);
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
  PmDeviceID devid = find_device(list_at(args, 0), type);

  if (devid == pmNoDevice && !c->pm->testing)
    return 1;

  char *sym = list_at(args, 1);
  char *name = list_at(args, 2);

  switch (type) {
  case 'i':
    list_append(c->pm->inputs, input_new(sym, name, devid));
    break;
  case 'o':
    list_append(c->pm->outputs, output_new(sym, name, devid));
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
  song *s = song_new(name);
  list_append(c->pm->all_songs->songs, s);
  c->song = s;
  c->patch = 0;
  c->conn = 0;
  return 0;
}

int load_notes(context *c) {
  char line[BUFSIZ];
  while (fgets(line, BUFSIZ, c->fp) != 0 && strncmp(line, "end", 3) != 0)
    song_append_notes(c->song, line);
  return 0;
}

int load_patch(context *c, char *line) {
  char *name = skip_first_word(line);
  patch *p = patch_new(name);
  list_append(c->song->patches, p);
  c->patch = p;
  c->conn = 0;
  return 0;
}

int load_connection(context *c, char *line) {
  list *args = comma_sep_args(line);
  input *in = find_by_sym(c->pm->inputs, (char *)list_at(args, 0));
  int in_chan = chan_from_word(list_at(args, 1));
  output *out = (output *)find_by_sym(c->pm->outputs, (char *)list_at(args, 2));
  int out_chan = chan_from_word(list_at(args, 3));

  connection *conn = connection_new(in, in_chan, out, out_chan);
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
  song_list *sl = song_list_new(name);
  list_append(c->pm->song_lists, sl);

  char song_name[BUFSIZ];
  while (fgets(line, BUFSIZ, c->fp) != 0 && strncmp(line, "end", 3) != 0) {
    strip_newline(song_name);
    list_append(sl->songs, find_song(c->pm->all_songs->songs, song_name));
  }
  c->song_list = sl;

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

input *find_by_sym(list *list, char *name) {
  for (int i = 0; i < list_length(list); ++i) {
    input *in = (input *)list_at(list, i);
    if (strcmp(name, in->sym) == 0)
      return in;
  }
  return 0;
}

song *find_song(list *list, char *name) {
  for (int i = 0; i < list_length(list); ++i) {
    song *s = (song *)list_at(list, i);
    if (strcmp(name, s->name) == 0)
      return s;
  }
  return 0;
}
