#include <sstream>
#include <stdlib.h>
#include <errno.h>
#include <err.h>
#include "portmidi.h"
#include "patchmaster.h"
#include "loader.h"
#include "formatter.h"
#include "debug.h"

static const markup org_mode_markup = {'*', "-*+", "#+"};
static const markup markdown_mode_markup = {'#', "-*+", "```"};

static const char * const default_patch_name = "Default Patch";
static const char * const whitespace = " \t";

Loader::Loader()
  : song(0)
{
}

Loader::~Loader() {
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
  pm->debug();

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

  List<char *> *cols = table_columns(line);
  if (strncmp(cols->at(0), "in", 2) == 0)
    load_instrument(*cols, INPUT);
  else if (strncmp(cols->at(0), "out", 3) == 0)
    load_instrument(*cols, OUTPUT);

  delete cols;
}

void Loader::parse_message_line(char *line) {
  if (is_header_level(line, 2)) {
    message = new Message(line + 3);
    pm->messages << message;
    return;
  }

  if (message == 0)             // introductory text
    return;

  message->messages << message_from_bytes(line);
}

void Loader::parse_trigger_line(char *line) {
  if (!is_table_row(line))
    return;

  List<char *> *cols = table_columns(line);
  Input *in = (Input *)find_by_sym(reinterpret_cast<List<Instrument *> &>(pm->inputs), cols->at(0));
  if (in == 0) {                // might be table header, not an error
    delete cols;
    return;
  }

  PmMessage trigger_msg = message_from_bytes(cols->at(1));
  Message *output_msg = 0;
  TriggerAction action;
  if (strcmp(cols->at(2), "next song") == 0)
    action = NEXT_SONG;
  else if (strcmp(cols->at(2), "prev song") == 0 || strcmp(cols->at(2), "previous song") == 0)
    action = PREV_SONG;
  else if (strcmp(cols->at(2), "next patch") == 0)
    action = NEXT_PATCH;
  else if (strcmp(cols->at(2), "prev patch") == 0 || strcmp(cols->at(2), "previous patch") == 0)
    action = PREV_PATCH;
  else if (strcmp(cols->at(2), "message") == 0) {
    action = MESSAGE;
    output_msg = find_message(pm->messages, cols->at(3));
    if (output_msg == 0) {
      ostringstream es;
      es << "trigger can not find message named " << cols->at(3);
      error_str = es.str();
      return;
    }
  }
  
  in->triggers << new Trigger(trigger_msg, action, output_msg);

  delete cols;
}

PmMessage Loader::message_from_bytes(const char *str) {
  int bytes[3] = {0, 0, 0};
  int i = 0;

  for (char *word = strtok((char *)str + strspn(str, whitespace), ", "); word != 0; word = strtok(0, ", ")) {
    if (i < 3) {
      word += strspn(word, whitespace);
      if (strlen(word) > 2 && strncmp(word, "0x", 2) == 0)
        bytes[i] = (int)strtol(word, 0, 16);
      else
        bytes[i] = atoi(word);
      ++i;
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
    load_notes_line(line);
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
    case 'f':
      load_filter(line);
      break;
    case 'm':
      load_map(line);
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

void Loader::load_instrument(List<char *> &cols, int type) {
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
    pm->inputs << new Input(sym, name, devid);
    break;
  case OUTPUT:
    pm->outputs << new Output(sym, name, devid);
    break;
  }

  return;
}

void Loader::load_message(char *line) {
  // TODO
  return;
}

void Loader::load_song(char *line) {
  if (song != 0)
    ensure_song_has_patch();

  Song *s = new Song(line);
  pm->all_songs->songs << s;
  song = s;
  patch = 0;
  conn = 0;
  notes_state = SKIPPING_BLANK_LINES;
  return;
}

void Loader::load_notes_line(char *line) {
  if (notes_state == SKIPPING_BLANK_LINES) {
    int start = strspn(line, whitespace);
    if (line[start] == 0)
      return;
  }

  notes_state = COLLECTING;
  song->append_notes(line);
  return;
}

void Loader::stop_collecting_notes() {
  if (song != 0) {              // remove trailing blank lines
    while (song->notes.length() > 0 && strlen(song->notes.last()) == 0)
      song->notes.remove_at(song->notes.length()-1);
  }
  notes_state = OUTSIDE;
}

void Loader::load_patch(char *line) {
  Patch *p = new Patch(line);
  song->patches << p;
  patch = p;
  conn = 0;
  stop_collecting_notes();
  return;
}

void Loader::load_connection(char *line) {
  List<char *> *args = comma_sep_args(line, false);
  Input *in = (Input *)find_by_sym(reinterpret_cast<List<Instrument *> &>(pm->inputs), args->at(0));
  if (in == 0) {
    instrument_not_found("input", args->at(0));
    return;
  }
  int in_chan = chan_from_word(args->at(1));
  Output *out = (Output *)find_by_sym(reinterpret_cast<List<Instrument *> &>(pm->outputs), args->at(2));
  if (out == 0) {
    instrument_not_found("output", args->at(2));
    return;
  }
  int out_chan = chan_from_word(args->at(3));

  conn = new Connection(in, in_chan, out, out_chan);
  patch->connections << conn;

  delete args;
  return;
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
  return;
}

void Loader::load_bank(char *line) {
  List<char *> *args = comma_sep_args(line, true);
  conn->prog.bank_msb = atoi(args->at(0));
  conn->prog.bank_lsb = atoi(args->at(1));
  return;
}

void Loader::load_xpose(char *line) {
  char *amount = skip_first_word(line);
  conn->xpose = atoi(amount);
  return;
}

void Loader::load_zone(char *line) {
  List<char *> *args = comma_sep_args(line, true);
  conn->zone.low = note_name_to_num(args->at(0));
  conn->zone.high = note_name_to_num(args->at(1));
  return;
}

void Loader::load_filter(char *line) {
  int controller = atoi(skip_first_word(line));
  conn->cc_maps[controller] = -1;
  return;
}

void Loader::load_map(char *line) {
  List<char *> *args = comma_sep_args(line, true);
  conn->cc_maps[atoi(args->at(0))] = atoi(args->at(1));
  delete args;
  return;
}

void Loader::load_song_list(char *line) {
  song_list = new SongList(line);
  pm->song_lists << song_list;
  return;
}

void Loader::load_song_list_song(char *line) {
  Song *s = find_song(pm->all_songs->songs, line);
  if (s == 0) {
    ostringstream es;
    es << "set list" << song_list->name << " can not find song named " << line;
    error_str = es.str();
    return;
  }

  song_list->songs << find_song(pm->all_songs->songs, line);
  return;
}

void Loader::ensure_song_has_patch() {
  if (song == 0 || song->patches.length() > 0)
    return;

  Patch *p = new Patch(default_patch_name);
  song->patches << p;

  for (int i = 0; i < pm->inputs.length(); ++i) {
    Input *in = pm->inputs[i];
    Output *out = (Output *)find_by_sym(reinterpret_cast<List<Instrument *> &>(pm->outputs),
                                        (char *)in->sym.c_str());
    if (out != 0) {
      Connection *conn = new Connection(in, -1, out, -1);
      p->connections << conn;
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
 * Skips first word on line, splits rest of line on commas, and returns the
 * list as a list of strings. The contents should NOT be freed, since they
 * are a destructive mutation of `line`.
 */
List<char *> *Loader::comma_sep_args(char *line, bool skip_word) {
  List<char *> *l = new List<char *>();
  char *args_start = skip_word ? skip_first_word(line) : line;

  for (char *word = strtok(args_start, ","); word != 0; word = strtok(0, ",")) {
    word += strspn(word, whitespace);
    l->append(word);
  }

  return l;
}

List<char *> *Loader::table_columns(char *line) {
  List<char *> *l = new List<char *>();

  line += strspn(line, whitespace);
  for (char *column = strtok(line, "|"); column != 0; column = strtok(0, "|"))
    l->append(trim(column));

  return l;
}

int Loader::chan_from_word(char *word) {
  return strcmp(word, "all") == 0 ? -1 : atoi(word) - 1;
}

PmDeviceID Loader::find_device(char *name, int device_type) {
  if (pm->testing)
    return pmNoDevice;

  int num_devices = Pm_CountDevices();
  for (int i = 0; i < num_devices; ++i) {
    const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
    if (device_type == INPUT && info->input && strcmp(name, info->name) == 0)
      return i;
    if (device_type == OUTPUT && info->output && strcmp(name, info->name) == 0)
      return i;
  }
  return pmNoDevice;
}

Instrument *Loader::find_by_sym(List<Instrument *> &list, char *name) {
  for (int i = 0; i < list.length(); ++i)
    if (list[i]->sym == name)
      return list[i];
  return 0;
}

Song *Loader::find_song(List<Song *> &list, char *name) {
  for (int i = 0; i < list.length(); ++i)
    if (list[i]->name == name)
      return list[i];
  return 0;
}

Message *Loader::find_message(List<Message *> &list, char *name) {
  for (int i = 0; i < list.length(); ++i)
    if (list[i]->name == name)
      return list[i];
  return 0;
}

bool Loader::is_header(const char *line, const char *header, int level) {
  if (!is_header_level(line, level))
    return false;
  return strcmp(line + level + 1, header) == 0;
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
  return strncmp(line, markup.block_marker_prefix, strlen(markup.block_marker_prefix)) == 0;
}

void Loader::determine_markup(const char *path) {
  char *extension = strrchr(path, '.');
  if (extension == 0)
    markup = org_mode_markup;
  else if (strncmp(extension, ".markdown", 9) == 0 || strncmp(extension, ".md", 3) == 0)
    markup = markdown_mode_markup;
  else
    markup = org_mode_markup;
}
