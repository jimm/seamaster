#ifndef LOAD_H
#define LOAD_H

#include "list.h"
#include "patchmaster.h"

typedef enum Section {
  IGNORE,
  INSTRUMENTS,
  MESSAGES,
  TRIGGERS,
  SONGS,
  SET_LISTS
} Section;

typedef enum NoteState {
  OUTSIDE,
  SKIPPING_BLANK_LINES,
  COLLECTING
} NoteState;

class Loader {
public:
  Loader(PatchMaster &pm);
  ~Loader();

  int load(const char *path);

private:
  FILE *fp;
  PatchMaster &pm;
  Section section;
  NoteState notes_state;
  Song *song;
  Patch *patch;
  Connection *conn;
  SongList *song_list;

  void clear();
  void enter_section(Section);
  void parse_line(char *);
  void parse_instrument_line(char *);
  void parse_message_line(char *);
  void parse_trigger_line(char *);
  void parse_song_line(char *);
  void parse_set_list_line(char *);

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

  char *skip_first_word(char *);
  List<char *> *comma_sep_args(char *, bool);
  int chan_from_word(char *);
  void strip_newline(char *);

  PmDeviceID find_device(char *, int);
  Instrument *find_by_sym(List<Instrument *> &, char *);
  Song *find_song(List<Song *> &, char *);
  bool is_org_mode_block_command(const char *);
};

#endif /* LOAD_H */
