#include <stdlib.h>
#include <string.h>
#include "song_list.h"

song_list *song_list_new(int id, char *name) {
  song_list *sl = malloc(sizeof(song_list));
  sl->id = id;
  sl->name = malloc(strlen(name)+1);
  strcpy(sl->name, name);
  sl->songs = list_new();
  return sl;
}

void song_list_free(song_list *sl) {
  if (sl->name)
    free(sl->name);
  free(sl);
}

char *song_list_name(song_list *sl) {
  return sl->name;
}
