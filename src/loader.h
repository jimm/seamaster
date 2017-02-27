#ifndef LOAD_H
#define LOAD_H

#include <string>
#include "list.h"
#include "patchmaster.h"

typedef enum InstrumentDirection {
  INPUT,
  OUTPUT
} InstrumentDirection;

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

typedef struct markup {
  char header_char;
  const char *list_chars;
  const char *block_marker_prefix;
} markup;

class Loader {
public:
  Loader();
  ~Loader();

  PatchMaster *load(const char *path, bool testing);
  bool has_error();
  string error();

private:
  FILE *fp;
  PatchMaster *pm;
  Section section;
  NoteState notes_state;
  Song *song;
  Patch *patch;
  Connection *conn;
  Message *message;
  SongList *song_list;
  string error_str;
  markup markup;

  void clear();
  void enter_section(Section);
  void parse_line(char *);
  void parse_instrument_line(char *);
  void parse_message_line(char *);
  void parse_trigger_line(char *);
  void parse_song_line(char *);
  void parse_set_list_line(char *);

  void load_instrument(List<char *> &, int);
  void instrument_not_found(const char *, const char *);
  void load_message(char *);
  PmMessage message_from_bytes(const char *);
  void load_song(char *);
  void load_notes_line(char *);
  void stop_collecting_notes();
  void load_patch(char *);
  void load_connection(char *);
  void load_bank(char *);
  void load_prog(char *);
  void load_xpose(char *);
  void load_zone(char *);
  void load_filter(char *);
  void load_map(char *);
  void load_song_list(char *);
  void load_song_list_song(char *);

  void ensure_song_has_patch();

  char *skip_first_word(char *);
  List<char *> *comma_sep_args(char *, bool);
  List<char *> *table_columns(char *);
  int chan_from_word(char *);
  void strip_newline(char *);
  char *trim(char *);

  PmDeviceID find_device(char *, int);
  Instrument *find_by_sym(List<Instrument *> &, char *);
  Song *find_song(List<Song *> &, char *);
  Message *find_message(List<Message *> &, char *);
  bool is_header(const char *, const char *, int);
  bool is_header_level(const char *, int);
  bool is_list_item(const char *);
  bool is_table_row(const char *);
  bool is_markup_block_command(const char *);
  void determine_markup(const char *);
};

#endif /* LOAD_H */
