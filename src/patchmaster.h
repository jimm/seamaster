#ifndef PATCHMASTER_H
#define PATCHMASTER_H

#include "song_list.h"
#include "list.h"
#include "input.h"
#include "output.h"
#include "message.h"

typedef struct cursor cursor;

typedef struct patchmaster {
  list *inputs;
  list *outputs;
  song_list *all_songs;
  list *song_lists;
  cursor *cursor;
  bool running;
  list *messages;
  list *triggers;
} patchmaster;

patchmaster *patchmaster_new();
void patchmaster_free(patchmaster *);

int patchmaster_load(patchmaster *, const char *);

void patchmaster_start(patchmaster *);
void patchmaster_stop(patchmaster *);

#ifdef DEBUG
void patchmaster_debug(patchmaster *);
#endif

#endif /* PATCHMASTER_H */