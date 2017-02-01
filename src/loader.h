#ifndef LOAD_H
#define LOAD_H

#include "list.h"
#include "patchmaster.h"

class Loader {
public:
  Loader(PatchMaster &pm);
  ~Loader();

  int load(const char *path);

private:
  FILE *fp;
  PatchMaster &pm;
  int section;
  int notes_state;
  Song *song;
  Patch *patch;
  Connection *conn;
  SongList *song_list;

  void clear();
  void parse_line(char *);
  void parse_instrument_line(char *);
  void parse_song_line(char *);
  void parse_set_list_line(char *);
  char *skip_first_word(char *);
  List<char *> *comma_sep_args(char *, bool);
  int chan_from_word(char *);
  void strip_newline(char *);

  PmDeviceID find_device(char *, char);
  Instrument *find_by_sym(List<Instrument *> &, char *);
  Song *find_song(List<Song *> &, char *);

  int load_instrument(char *, int);
  int load_message(char *);
  int load_trigger(char *);
  int load_song(char *);
  int load_notes_line(char *);
  void stop_collecting_notes();
  int load_patch(char *);
  int load_connection(char *);
  int load_bank(char *);
  int load_prog(char *);
  int load_xpose(char *);
  int load_zone(char *);
  int load_filter(char *);
  int load_map(char *);
  int load_song_list(char *);
  int load_song_list_song(char *);
};

#endif /* LOAD_H */