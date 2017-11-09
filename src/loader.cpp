#include <sstream>
#include <stdlib.h>
#include <errno.h>
#include <err.h>
#include "portmidi.h"
#include "patchmaster.h"
#include "cursor.h"
#include "loader.h"
#include "formatter.h"

using namespace std;

static const markup org_mode_markup = {'*', "-*+", "#+"};
static const markup markdown_mode_markup = {'#', "-*+", "```"};

static const char * const default_patch_name = "Default Patch";
static const char * const whitespace = " \t";

Loader::Loader()
  : song(0)
{
}

Loader::~Loader() {
  clear_notes();
}

PatchMaster *Loader::load(const char *path, bool testing) {
  int retval = 0;
  char line[BUFSIZ];

  error_str = "";

  fp = fopen(path, "r");
  if (fp == 0) {
    error_str = strerror(errno);
    return 0;
  }

  determine_markup(path);

  PatchMaster *old_pm = PatchMaster_instance();
  pm = new PatchMaster();    // side-effect: PatchMaster static instance set
  pm->loaded_from_file = path;
  pm->testing = testing;
  clear();
  while (!has_error() && fgets(line, BUFSIZ, fp) != 0) {
    strip_newline(line);
    parse_line(line);
  }
  if (song != 0)
    ensure_song_has_patch();

  fclose(fp);

  if (old_pm)
    pm->cursor->attempt_goto(old_pm->cursor);

  return pm;
}

bool Loader::has_error() {
  return error_str != "";
}

string Loader::error() {
  return error_str;
}

void Loader::clear() {
  if (song != 0)
    ensure_song_has_patch();

  section = IGNORE;
  notes_state = OUTSIDE;
  song_list = 0;
  song = 0;
  patch = 0;
  conn = 0;
  message = 0;
}

void Loader::enter_section(Section sec) {
  clear();
  section = sec;
}

void Loader::parse_line(char *line) {
  int start = 0;
  if (notes_state == OUTSIDE) {
    start = strspn(line, whitespace);
    if (line[start] == 0 || is_markup_block_command(line))
      return;
  }
  else if (is_markup_block_command(line))
    return;

  // Header lines must start at beginning of the line, so don't skip past
  // whitespace quite yet.

  if (is_header(line, "Instruments", 1)) {
    enter_section(INSTRUMENTS);
    return;
  }
  if (is_header(line, "Messages", 1)) {
    enter_section(MESSAGES);
    return;
  }
  if (is_header(line, "Triggers", 1)) {
    enter_section(TRIGGERS);
    return;
  }
  if (is_header(line, "Songs", 1)) {
    enter_section(SONGS);
    return;
  }
  if (is_header(line, "Set Lists", 1)) {
    enter_section(SET_LISTS);
    return;
  }
  if (is_header_level(line, 1)) {
    enter_section(IGNORE);
    return;
  }

  // Now we can strip leading whitespace.
  line += start;

  switch (section) {
  case INSTRUMENTS:
    parse_instrument_line(line);
    break;
  case MESSAGES:
    parse_message_line(line);
    break;
  case TRIGGERS:
    parse_trigger_line(line);
    break;
  case SONGS:
    parse_song_line(line);
    break;
  case SET_LISTS:
    parse_set_list_line(line);
    break;
  default:
    break;
  }
}

void Loader::parse_instrument_line(char *line) {
  if (!is_table_row(line))
    return;

  vector<char *> cols;
  table_columns(line, cols);
  if (strncasecmp(cols[0], "in", 2) == 0)
    load_instrument(cols, INPUT);
  else if (strncasecmp(cols[0], "out", 3) == 0)
    load_instrument(cols, OUTPUT);
}

void Loader::parse_message_line(char *line) {
  if (is_header_level(line, 2)) {
    message = new Message(line + 3);
    pm->messages.push_back(message);
    return;
  }

  if (message == 0)             // introductory text
    return;

  message->messages.push_back(message_from_bytes(line));
}

void Loader::parse_trigger_line(char *line) {
  if (!is_table_row(line))
    return;

  vector<char *> cols;
  table_columns(line, cols);
  Input *in = (Input *)find_by_sym(reinterpret_cast<vector<Instrument *> &>(pm->inputs), cols[0]);
  if (in == 0) {                // might be table header, not an error
    return;
  }

  PmMessage trigger_msg = message_from_bytes(cols[1]);
  Message *output_msg = 0;
  TriggerAction action;
  if (strncasecmp(cols[2], "next song", 9) == 0)
    action = NEXT_SONG;
  else if (strncasecmp(cols[2], "prev song", 9) == 0 || strncasecmp(cols[2], "previous song", 13) == 0)
    action = PREV_SONG;
  else if (strncasecmp(cols[2], "next patch", 10) == 0)
    action = NEXT_PATCH;
  else if (strncasecmp(cols[2], "prev patch", 10) == 0 || strncasecmp(cols[2], "previous patch", 14) == 0)
    action = PREV_PATCH;
  else if (strncasecmp(cols[2], "message", 7) == 0) {
    action = MESSAGE;
    output_msg = find_message(pm->messages, cols[3]);
    if (output_msg == 0) {
      ostringstream es;
      es << "trigger can not find message named " << cols[3];
      error_str = es.str();
      return;
    }
  }
  
  in->triggers.push_back(new Trigger(trigger_msg, action, output_msg));
}

PmMessage Loader::message_from_bytes(const char *str) {
  int bytes[3] = {0, 0, 0};
  int i = 0;

  for (char *word = strtok((char *)str + strspn(str, whitespace), ", "); word != 0; word = strtok(0, ", ")) {
    if (i < 3) {
      word += strspn(word, whitespace);
      if (strlen(word) > 2 && strncasecmp(word, "0x", 2) == 0) {
        bytes[i] = (int)strtol(word, 0, 16);
        ++i;
      }
      else if (isdigit(word[0])) {
        bytes[i] = atoi(word);
        ++i;
      }
    }
  }

  return Pm_Message(bytes[0], bytes[1], bytes[2]);
}

void Loader::parse_song_line(char *line) {
  if (is_header_level(line, 2))
    load_song(line + 3);
  else if (is_header_level(line, 3))
    load_patch(line + 4);
  else if (is_header_level(line, 4))
    load_connection(line + 5);
  else if (notes_state != OUTSIDE)
    save_notes_line(line);
  else if (is_list_item(line) && conn != 0) {
    line += 2;
    char ch = line[0];
    switch (ch) {
    case 'b':
      load_bank(line);
      break;
    case 'p':
      load_prog(line);
      break;
    case 'x':
      load_xpose(line);
      break;
    case 'z':
      load_zone(line);
      break;
    case 'c':
      load_controller(line);
      break;
    }
  }
}

void Loader::parse_set_list_line(char *line) {
  if (is_header_level(line, 2))
    load_song_list(line + 3);
  else if (is_list_item(line))
    load_song_list_song(line + 2);
}

void Loader::load_instrument(vector<char *> &cols, int type) {
  PmDeviceID devid = find_device(cols[1], type);

  if (devid == pmNoDevice && !pm->testing) {
    ostringstream es;
    es << "MIDI port " << cols[1] << " not found";
    error_str = es.str();
    return;
  }

  char *sym = cols[2];
  char *name = cols[3];

  switch (type) {
  case INPUT:
    pm->inputs.push_back(new Input(sym, name, devid));
    break;
  case OUTPUT:
    pm->outputs.push_back(new Output(sym, name, devid));
    break;
  }
}

void Loader::load_message(char *line) {
  // TODO
}

void Loader::load_song(char *line) {
  if (song != 0)
    ensure_song_has_patch();

  Song *s = new Song(line);
  pm->all_songs->songs.push_back(s);
  song = s;
  patch = 0;
  conn = 0;
  start_collecting_notes();
}

void Loader::save_notes_line(char *line) {
  if (notes_state == SKIPPING_BLANK_LINES) {
    int start = strspn(line, whitespace);
    if (line[start] == 0)
      return;
  }

  notes_state = COLLECTING;
  char *str = (char *)malloc(strlen(line) + 1);
  strncpy(str, line, strlen(line)+1);
  notes.push_back(str);
}

void Loader::start_collecting_notes() {
  notes_state = SKIPPING_BLANK_LINES;
  clear_notes();
}

void Loader::stop_collecting_notes() {
  // remove trailing blank lines
  while (!notes.empty() && strlen(notes.back()) == 0) {
    free(notes.back());
    notes.erase(notes.end() - 1);
  }
  notes_state = OUTSIDE;
}

void Loader::load_patch(char *line) {
  stop_collecting_notes();
  if (!notes.empty() && song != 0) {
    song->take_notes(notes);
    notes.clear();              // do not dealloc
  }

  Patch *p = new Patch(line);
  song->patches.push_back(p);
  patch = p;
  conn = 0;

  start_collecting_notes();
}

void Loader::load_connection(char *line) {
  stop_collecting_notes();
  if (!notes.empty() && conn == 0) // first conn, interpret start/stop in notes
    start_and_stop_messages_from_notes();

  vector<char *> args;
  comma_sep_args(line, false, args);
  Input *in = (Input *)find_by_sym(reinterpret_cast<vector<Instrument *> &>(pm->inputs), args[0]);
  if (in == 0) {
    instrument_not_found("input", args[0]);
    return;
  }
  int in_chan = chan_from_word(args[1]);
  Output *out = (Output *)find_by_sym(reinterpret_cast<vector<Instrument *> &>(pm->outputs), args[2]);
  if (out == 0) {
    instrument_not_found("output", args[2]);
    return;
  }
  int out_chan = chan_from_word(args[3]);

  conn = new Connection(in, in_chan, out, out_chan);
  patch->connections.push_back(conn);
}

void Loader::start_and_stop_messages_from_notes() {
  StartStopState state = UNSTARTED;
  for (vector<char *>::iterator i = notes.begin(); i != notes.end(); ++i) {
    char *str = *i + strspn(*i, whitespace);
    if (strlen(str) == 0)       
      continue;

    if (strncasecmp(str, "start", 5) == 0)
      state = START_MESSAGES;
    else if (strncasecmp(str, "stop", 4) == 0)
      state = STOP_MESSAGES;
    else {
      switch (state) {
      case START_MESSAGES:
        patch->start_messages.push_back(message_from_bytes(str));
        break;
      case STOP_MESSAGES:
        patch->stop_messages.push_back(message_from_bytes(str));
        break;
      case UNSTARTED:
        break;
      }
    }
  }

  clear_notes();
}

void Loader::instrument_not_found(const char *type_name, const char *sym) {
    ostringstream es;
    es << "song " << song->name
       << ", patch " << patch->name
       << ": " << type_name << ' ' << sym
       << " not found";
    error_str = es.str();
}

void Loader::load_prog(char *line) {
  char *prog_chg = skip_first_word(line);
  conn->prog.prog = atoi(prog_chg);
}

void Loader::load_bank(char *line) {
  vector<char *> args;

  comma_sep_args(line, true, args);
  conn->prog.bank_msb = atoi(args[0]);
  conn->prog.bank_lsb = atoi(args[1]);
}

void Loader::load_xpose(char *line) {
  char *amount = skip_first_word(line);
  conn->xpose = atoi(amount);
}

void Loader::load_zone(char *line) {
  vector<char *> args;
  comma_sep_args(line, true, args);
  conn->zone.low = note_name_to_num(args[0]);
  conn->zone.high = note_name_to_num(args[1]);
}

void Loader::load_controller(char *line) {
  vector<char *> args;
  whitespace_sep_args(line, true, args);
  Controller &cc = conn->cc_maps[atoi(args[0])];
  whitespace_sep_args(line, true, args);
  for (vector<char *>::iterator i = args.begin(); i != args.end(); ++i) {
    switch ((*i)[0]) {
    case 'f':                   // filter
      cc.filtered = true;
      break;
    case 'm':                   // map
      cc.translated_cc_num = atoi(*++i);
      break;
    case 'l':                   // limit
      cc.min = atoi(*++i);
      cc.max = atoi(*++i);
      break;
    }
  }
}

void Loader::load_song_list(char *line) {
  song_list = new SongList(line);
  pm->song_lists.push_back(song_list);
}

void Loader::load_song_list_song(char *line) {
  Song *s = find_song(pm->all_songs->songs, line);
  if (s == 0) {
    ostringstream es;
    es << "set list " << song_list->name << " can not find song named " << line;
    error_str = es.str();
    return;
  }

  song_list->songs.push_back(find_song(pm->all_songs->songs, line));
}

void Loader::ensure_song_has_patch() {
  if (song == 0 || !song->patches.empty())
    return;

  Patch *p = new Patch(default_patch_name);
  song->patches.push_back(p);

  for (vector<Input *>::iterator i = pm->inputs.begin(); i != pm->inputs.end(); ++i) {
    Input *in = *i;
    Output *out = (Output *)find_by_sym(reinterpret_cast<vector<Instrument *> &>(pm->outputs),
                                        (char *)in->sym.c_str());
    if (out != 0) {
      Connection *conn = new Connection(in, -1, out, -1);
      p->connections.push_back(conn);
    }
  }
}

void Loader::strip_newline(char *line) {
  int len = strlen(line);
  if (line[len-1] == '\n')
    line[len-1] = 0;
}

char *Loader::trim(char *line) {
  line += strspn(line, whitespace);
  char *p = line + strlen(line) - 1;

  while (p >= line && isspace(*p))
    --p;
  if (p >= line)
    *(p+1) = 0;
  return line;
}

char *Loader::skip_first_word(char *line) {
  char *after_leading_spaces = line + strspn(line, whitespace);
  char *after_word = after_leading_spaces + strcspn(line, whitespace);
  return after_word + strspn(after_word, whitespace);
}

/*
 * Skips first word on line, splits rest of line on whitespace, and returns
 * the list as a list of strings. The contents should NOT be freed, since
 * they are a destructive mutation of `line`.
 */
void Loader::whitespace_sep_args(char *line, bool skip_word, vector<char *> &v) {
  char *args_start = skip_word ? skip_first_word(line) : line;

  for (char *word = strtok(args_start, whitespace); word != 0; word = strtok(0, whitespace)) {
    word += strspn(word, whitespace);
    v.push_back(word);
  }
}

/*
 * Skips first word on line, splits rest of line on commas, and returns the
 * list as a list of strings. The contents should NOT be freed, since they
 * are a destructive mutation of `line`.
 */
void Loader::comma_sep_args(char *line, bool skip_word, vector<char *> &v) {
  char *args_start = skip_word ? skip_first_word(line) : line;

  for (char *word = strtok(args_start, ","); word != 0; word = strtok(0, ",")) {
    word += strspn(word, whitespace);
    v.push_back(word);
  }
}

void Loader::table_columns(char *line, vector<char *> &v) {
  line += strspn(line, whitespace);
  for (char *column = strtok(line, "|"); column != 0; column = strtok(0, "|"))
    v.push_back(trim(column));
}

int Loader::chan_from_word(char *word) {
  return strncasecmp(word, "all", 3) == 0 ? -1 : atoi(word) - 1;
}

PmDeviceID Loader::find_device(char *name, int device_type) {
  if (pm->testing)
    return pmNoDevice;

  int num_devices = Pm_CountDevices();
  for (int i = 0; i < num_devices; ++i) {
    const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
    if (device_type == INPUT && info->input && strncasecmp(name, info->name, strlen(info->name)) == 0)
      return i;
    if (device_type == OUTPUT && info->output && strncasecmp(name, info->name, strlen(info->name)) == 0)
      return i;
  }
  return pmNoDevice;
}

Instrument *Loader::find_by_sym(vector<Instrument *> &list, char *name) {
  for (vector<Instrument *>::iterator i = list.begin(); i != list.end(); ++i)
    if (strncasecmp((*i)->sym.c_str(), name, (*i)->sym.length()) == 0)
      return *i;
  return 0;
}

Song *Loader::find_song(vector<Song *> &list, char *name) {
  for (vector<Song *>::iterator i = list.begin(); i != list.end(); ++i)
    if (strncasecmp((*i)->name.c_str(), name, (*i)->name.length()) == 0)
      return *i;
  return 0;
}

Message *Loader::find_message(vector<Message *> &list, char *name) {
  for (vector<Message *>::iterator i = list.begin(); i != list.end(); ++i)
    if (strncasecmp((*i)->name.c_str(), name, (*i)->name.length()) == 0)
      return *i;
  return 0;
}

bool Loader::is_header(const char *line, const char *header, int level) {
  if (!is_header_level(line, level))
    return false;
  return strncasecmp(line + level + 1, header, strlen(header)) == 0;
}

bool Loader::is_header_level(const char *line, int level) {
  for (int i = 0; i < level; ++i)
    if (line[i] != markup.header_char)
      return false;
  return line[level] == ' ';
}

// If item_begins_with is not 0, only return true if it matches.
bool Loader::is_list_item(const char *line) {
  return strrchr(markup.list_chars, line[0]) != 0 && line[1] == ' ';
}

bool Loader::is_table_row(const char *line) {
  int start = strspn(line, whitespace);
  return line[start] == '|' &&
    // make sure this is not a separator line
    (line[start+2] != '-' || strchr(" :-", line[start+1]) == 0);
}

bool Loader::is_markup_block_command(const char *line) {
  return strncasecmp(line, markup.block_marker_prefix, strlen(markup.block_marker_prefix)) == 0;
}

void Loader::determine_markup(const char *path) {
  const char *extension = strrchr(path, '.');
  if (extension == 0)
    markup = org_mode_markup;
  else if (strncasecmp(extension, ".markdown", 9) == 0 || strncasecmp(extension, ".md", 3) == 0)
    markup = markdown_mode_markup;
  else
    markup = org_mode_markup;
}

void Loader::clear_notes() {
  for (vector<char *>::iterator i = notes.begin(); i != notes.end(); ++i)
    free(*i);
  notes.clear();
}
