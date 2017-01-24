#ifndef SONG_LIST_H
#define SONG_LIST_H

#include "list.h"
#include "song.h"

typedef struct song_list {
  int id;
  char *name;
  list *songs;
} song_list;

song_list *song_list_new(int id, char *name);
void song_list_free(song_list *);

char *song_list_name(song_list *);

#endif /* SONGLIST_LIST_H */
