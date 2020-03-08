#ifndef LOAD_H
#define LOAD_H

#include <string>
#include <vector>
#include "patchmaster.h"

using namespace std;

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

typedef enum StartStopState {
  UNSTARTED,
  START_MESSAGES,
  STOP_MESSAGES
} StartStopState;

typedef struct markup {
  char header_char;
  const char *list_chars;
  const char *block_marker_prefix;
  const char *block_line_prefix;
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
  SetList *set_list;
  string error_str;
  markup markup;
  string notes;

  void clear();
  void enter_section(Section);
  void parse_line(char *);
  void parse_instrument_line(char *);
  void parse_message_line(char *);
  void parse_trigger_line(char *);
  void parse_song_line(char *);
  void parse_set_list_line(char *);

  void load_instrument(vector<char *> &, int);
  void instrument_not_found(const char *, const char *);
  void load_message(char *);
  void load_song(char *);
  void save_notes_line(char *);
  void start_collecting_notes();
  void stop_collecting_notes();
  void load_patch(char *);
  void load_connection(char *);
  void start_and_stop_messages_from_notes();
  void load_bank(char *);
  void load_prog(char *);
  void load_xpose(char *);
  void load_zone(char *);
  void load_controller(char *);
  void load_pass_through_sysex(char *);
  void load_set_list(char *);
  void load_set_list_song(char *);

  void ensure_song_has_patch();

  char *skip_first_word(char *);
  void whitespace_sep_args(char *, bool, vector<char *> &);
  void comma_sep_args(char *, bool, vector<char *> &);
  void table_columns(char *, vector<char *> &);
  int chan_from_word(char *);
  void strip_newline(char *);
  char *trim(char *);

  PmDeviceID find_device(char *, int);
  int compare_device_names(char *, char *);
  Instrument *find_by_name(vector<Instrument *> &, char *);
  Song *find_song(vector<Song *> &, char *);
  Message *find_message(vector<Message *> &, char *);
  bool is_header(const char *, const char *, int);
  bool is_header_level(const char *, int);
  bool is_list_item(const char *);
  bool is_table_row(const char *);
  bool is_markup_block_command(const char *);
  bool is_markup_block_line(const char *);
  void determine_markup(const char *);
  void clear_notes();
};

#endif /* LOAD_H */
