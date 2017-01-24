#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <err.h>
#include "portmidi.h"
#include "patchmaster.h"
#include "load.h"

typedef struct context {
  FILE *fp;
  patchmaster *pm;
  song *song;
  patch *patch;
  song_list *song_list;
} context;

void parse_line(context *, char *);
list *comma_sep_args(char *);
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
#ifdef DEBUG
  patchmaster_debug(pm);
#endif
  return 0;
}

void parse_line(context *c, char *line) {
  int start = strspn(line, " \t");
  if (line[start] == 0 || line[start] == '#') /* whitespace only or comment */
    return;

  int ch = line[start];
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
    load_song(c, line);
    break;
  case 'p':
    load_patch(c, line);
    break;
  case 'c':
    load_connection(c, line);
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
  switch (type) {
  case 'i':
    list_append(c->pm->inputs, input_new(list_at(args, 1), list_at(args, 2),
                                         find_device(list_at(args, 0), 'i')));
    break;
  case 'o':
    list_append(c->pm->outputs, output_new(list_at(args, 1), list_at(args, 2),
                                           find_device(list_at(args, 0), 'o')));
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
  char *name = line + strcspn(line, " ");
  song *s = song_new(name);
  list_append(c->pm->all_songs->songs, s);
  c->song = s;
  return 0;
}

int load_notes(context *c) {
  char line[BUFSIZ];
  while (fgets(line, BUFSIZ, c->fp) != 0 && strncmp(line, "end_notes", 9) != 0)
    song_append_notes(c->song, line);
  return 0;
}

int load_patch(context *c, char *line) {
  char *name = line + strcspn(line, " ");
  patch *p = patch_new(name);
  list_append(c->song->patches, p);
  c->patch = p;
  return 0;
}

int load_connection(context *c, char *line) {
  list *args = comma_sep_args(line);
  input *in = find_by_sym(c->pm->inputs, (char *)list_at(args, 0));
  int in_chan = strcmp(list_at(args, 1), "all") == 0 ? -1 : atoi(list_at(args, 1)) - 1;
  output *out = (output *)find_by_sym(c->pm->outputs, (char *)list_at(args, 2));
  int out_chan = strcmp(list_at(args, 3), "all") == 0 ? -1 : atoi(list_at(args, 3)) - 1;
  connection *conn = connection_new(in, in_chan, out, out_chan);
  list_append(c->patch->connections, conn);
  return 0;
}

int load_song_list(context *c, char *line) {
  char *name = line + strcspn(line, " ");
  song_list *sl = song_list_new(name);
  list_append(c->pm->song_lists, sl);

  char song_name[BUFSIZ];
  while (fgets(line, BUFSIZ, c->fp) != 0 && strncmp(line, "end_list", 8) != 0) {
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

/*
 * Skips first word on line, splits rest of line on commas, and returns the
 * list as a list of strings. The contents should NOT be freed, since they
 * are a destructive mutation of `line`.
 */
list *comma_sep_args(char *line) {
  list *l = list_new();
  int word_end = strcspn(line, " ");

  char *word;
  for (word = strtok(line+word_end, ","); word != 0; word = strtok(0, ",")) {
    word += strcspn(word, " \t");
    list_append(l, word);
  }

  return l;
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
