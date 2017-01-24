#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "portmidi.h"
#include "patchmaster.h"
#include "sqlite3.h"
#include "load.h"

typedef struct context {
  patchmaster *pm;
  song *song;
  patch *patch;
  song_list *song_list;
} context;

int int_from_col(char *);
input *find_by_id(list *, int);
PmDeviceID find_device(char *, char);

int load_instruments(sqlite3 *, context *);
int load_messages(sqlite3 *, context *);
int load_songs(sqlite3 *, context *);
int load_patches(sqlite3 *, context *);
int load_connections(sqlite3 *, context *);
int load_song_lists(sqlite3 *, context *);
int load_song_list_songs(sqlite3 *db, context *context);

int load(patchmaster *pm, const char *path) {
  sqlite3 *db;
  int rc, retval;
  context context;

  rc = sqlite3_open(path, &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "error opening database %s: %s\n", path, sqlite3_errmsg(db));
    return -1;
  }

  context.pm = pm;
  context.song = 0;
  context.patch = 0;
  context.song_list = 0;
  retval = -1;
  if (load_instruments(db, &context) == 0 &&
      load_songs(db, &context) == 0 &&
      load_song_lists(db, &context) == 0) {
    retval = 0;
  }
  sqlite3_close(db);
  return retval;
}

int load_instruments_callback(void *cptr, int argc, char **argv,
                                          char **col_names) {
  context *c = (context *)cptr;
  if (int_from_col(argv[4]) == 1) {
    input *input = input_new(int_from_col(argv[0]), argv[2], argv[1],
                             find_device(argv[3], 'i'));
    list_append(c->pm->inputs, input);
  }
  if (int_from_col(argv[5]) == 1) {
    output *output = output_new(int_from_col(argv[0]), argv[2], argv[1],
                                find_device(argv[3], 'o'));
    list_append(c->pm->outputs, output);
  }
  return 0;
}   

int load_instruments(sqlite3 *db, context *context) {
  char *errmsg = 0;
  int rc;

  rc = sqlite3_exec(db,
                    "select id, name, short_name, port_name, input, output from instruments",
                    load_instruments_callback, context, &errmsg);
  if (rc != 0) {
    fprintf(stderr, "error loading instruments: %s\n", errmsg);
    sqlite3_free(errmsg);
    return -1;
  }
  return 0;
}

int load_messages(sqlite3 *db, context *context) {
  int rc;
  sqlite3_stmt *stmt;

  char *sql = "select id, name, length, bytes from messages";
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
  if (rc != SQLITE_OK)
    return -1;

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    int id = sqlite3_column_int(stmt, 0);
    const unsigned char *name = sqlite3_column_text(stmt, 1);
    int num_bytes = sqlite3_column_int(stmt, 2);
    const void *bytes = sqlite3_column_blob(stmt, 3);
    message *m = message_new(id, (char *)name, num_bytes, (byte *)bytes);
    list_append(context->pm->messages, m);
  }

  rc = sqlite3_finalize(stmt);
  return rc == SQLITE_OK ? 0 : -1;
}

int load_songs_callback(void *cptr, int argc, char **argv,
                                    char **col_names) {
  context *c = (context *)cptr;
  song *song = song_new(int_from_col(argv[0]), argv[1], argv[2]);
  list_append(c->pm->all_songs->songs, song);
  return 0;
}

int load_songs(sqlite3 *db, context *context) {
  char *errmsg = 0;
  int rc;

  rc = sqlite3_exec(db, "select id, name, notes from songs order by name",
                    load_songs_callback, context, &errmsg);
  if (rc != 0) {
    fprintf(stderr, "error loading songs: %s\n", errmsg);
    sqlite3_free(errmsg);
    return -1;
  }

  for (int i = 0; i < list_length(context->pm->all_songs->songs); ++i) {
    song *s = (song *)list_at(context->pm->all_songs->songs, i);
    context->song = s;
    load_patches(db, context);
  }

  return 0;
}

int load_patches_callback(void *cptr, int argc, char **argv,
                                      char **col_names) {
  context *c = (context *)cptr;
  patch *patch = patch_new(int_from_col(argv[0]), argv[1]);
  list_append(c->song->patches, patch);
  return 0;
}

int load_patches(sqlite3 *db, context *context) {
  char sql[BUFSIZ];
  char *errmsg = 0;
  int rc;

  sprintf(sql, "select id, name from patches where song_id = %d order by position",
          context->song->id);
  rc = sqlite3_exec(db, sql, load_patches_callback, context,
                    &errmsg);
  if (rc != 0) {
    fprintf(stderr, "error loading patches: %s\n", errmsg);
    sqlite3_free(errmsg);
    return -1;
  }

  for (int i = 0; i < list_length(context->song->patches); ++i) {
    patch *p = (patch *)list_at(context->song->patches, i);
    context->patch = p;
    load_connections(db, context);
  }

  return 0;
}

int load_connections_callback(void *cptr, int argc, char **argv,
                                          char **col_names) {
  context *c = (context *)cptr;
  connection *conn =
    connection_new(int_from_col(argv[0]),
                   find_by_id(c->pm->inputs, int_from_col(argv[1])),
                   int_from_col(argv[2]),
                   (output *)find_by_id(c->pm->outputs,int_from_col(argv[3])),
                   int_from_col(argv[4]),
                   int_from_col(argv[5]), int_from_col(argv[6]), /* bank */
                   int_from_col(argv[7]), /* prog chg */
                   int_from_col(argv[8]), int_from_col(argv[9]), /* zone */
                   int_from_col(argv[10]));              /* xpose */
  list_append(c->patch->connections, conn);

  return 0;
}

int load_connections(sqlite3 *db, context *context) {
  char sql[BUFSIZ];
  char *errmsg = 0;
  int rc;

  sprintf(sql, "select id, input_id, input_chan, output_id, output_chan,"
          " bank_msb, bank_lsb, prog_chg, zone_low, zone_high, xpose"
          " from connections where patch_id = %d",
          context->patch->id);
  rc = sqlite3_exec(db, sql, load_connections_callback, context,
                    &errmsg);
  if (rc != 0) {
    fprintf(stderr, "error loading connections: %s\n", errmsg);
    sqlite3_free(errmsg);
    return -1;
  }

  return 0;
}

int load_song_lists_callback(void *cptr, int argc, char **argv,
                                         char **col_names) {
  context *c = (context *)cptr;
  song_list *song_list = song_list_new(int_from_col(argv[0]), argv[1]);
  list_append(c->pm->song_lists, song_list);
  return 0;
}

int load_song_lists(sqlite3 *db, context *context) {
  char sql[BUFSIZ];
  char *errmsg = 0;
  int rc;

  rc = sqlite3_exec(db, "select id, name from song_lists",
                    load_song_lists_callback, context, &errmsg);
  if (rc != 0) {
    fprintf(stderr, "error loading song lists: %s\n", errmsg);
    sqlite3_free(errmsg);
    return -1;
  }

  // Skip song list 0, which is special "all songs" lists that already
  // contains all the songs we've loaded
  for (int i = 1; i < list_length(context->pm->song_lists); ++i) {
    song_list *sl = (song_list *)list_at(context->pm->song_lists, i);
    context->song_list = sl;
    load_song_list_songs(db, context);
  }

  return 0;
}

int load_song_list_songs_callback(void *cptr, int argc, char **argv,
                                              char **col_names) {
  context *c = (context *)cptr;
  song_list *sl = c->song_list;
  song *s = (song *)find_by_id(c->pm->all_songs->songs, int_from_col(argv[0]));
  list_append(sl->songs, s);
  return 0;
}

int load_song_list_songs(sqlite3 *db, context *context) {
  char sql[BUFSIZ], *errmsg = 0;
  int rc;

  song_list *sl = context->song_list;
  sprintf(sql,
          "select song_id from song_lists_songs where song_list_id = %d order by position",
          sl->id);
  rc = sqlite3_exec(db, sql, load_song_list_songs_callback, context,
                    &errmsg);
  if (rc != 0) {
    fprintf(stderr, "error loading song list songs: %s\n", errmsg);
    sqlite3_free(errmsg);
    return -1;
  }
  return 0;
}

int int_from_col(char *s) {
  return s ? atoi(s) : -1;
}

input *find_by_id(list *list, int val) {
  for (int i = 0; i < list_length(list); ++i) {
    input *in = (input *)list_at(list, i);
    if (in->id == val)
      return in;
  }
  return 0;
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
