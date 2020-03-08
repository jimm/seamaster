#include <string>
#include <fstream>
#include <streambuf>
#include <stdlib.h>
#include <strings.h>
#include <errno.h>
#include <err.h>
#include <portmidi.h>
#include <sqlite3.h>
#include "patchmaster.h"
#include "cursor.h"
#include "storage.h"
#include "formatter.h"

#define INT_OR_NULL(col_num, null_val) \
  (sqlite3_column_type(stmt, col_num) == SQLITE_NULL ? null_val : sqlite3_column_int(stmt, col_num))
#define TEXT_OR_NULL(col_num, null_val) \
  (sqlite3_column_type(stmt, col_num) == SQLITE_NULL ? null_val : (const char *)sqlite3_column_text(stmt, col_num))
#define BIND_ID_OR_NULL(col_num, obj_ptr) \
  (obj_ptr == nullptr ? sqlite3_bind_null(stmt, col_num) : sqlite3_bind_int(stmt, col_num, obj_ptr->id()))
#define BIND_INT_OR_NULL(col_num, val, nullval) \
  (val == nullval ? sqlite3_bind_null(stmt, col_num) : sqlite3_bind_int(stmt, col_num, val))

using namespace std;

Storage::Storage(const char *path) {
  int status = sqlite3_open(path, &db);
  if (status != 0) {
    db = nullptr;
    char error_buf[BUFSIZ];
    sprintf(error_buf,  "error opening database file %s", path);
    error_str = error_buf;
  }
}

Storage::~Storage() {
  if (db != nullptr)
    sqlite3_close(db);
}

// Does not stop old pm or start new pm.
PatchMaster *Storage::load(bool testing) {
  PatchMaster *old_pm = PatchMaster_instance();
  int status;

  if (db == nullptr)
    return old_pm;

  pm = new PatchMaster();    // side-effect: PatchMaster static instance set

  max_patch_id = max_conn_id = -1;
  load_instruments();
  load_messages();
  load_triggers();
  load_songs();
  load_set_lists();
  create_default_patches();

  pm->testing = testing;
  return pm;
}

void Storage::save(PatchMaster *patchmaster, bool testing) {
  if (db == nullptr)
    return;

  initialize(testing);
  if (has_error())
    return;

  pm = patchmaster;
  save_instruments();
  save_messages();
  save_triggers();
  save_songs();
  save_set_lists();
}

bool Storage::has_error() {
  return error_str != "";
}

string Storage::error() {
  return error_str;
}

void Storage::initialize(bool testing) {
  if (testing) {
    initialize_with_schema_file("db/schema.sql");
    return;
  }

  char *xdg_config_home = getenv("XDG_CONFIG_HOME");
  string config_path;
  if (xdg_config_home == nullptr) {
    config_path = getenv("HOME");
    config_path += "/.config";
  }
  else
    config_path = xdg_config_home;
  config_path += "/seamaster/schema.sql";

  initialize_with_schema_file(config_path);
}

void Storage::initialize_with_schema_file(string config_path) {
  // read schema file and execute
  char *error_buf;
  std::ifstream schema_t(config_path);
  std::string schema_sql((std::istreambuf_iterator<char>(schema_t)),
                         std::istreambuf_iterator<char>());
  int status = sqlite3_exec(db, schema_sql.c_str(), nullptr, nullptr, &error_buf);
  if (status != 0) {
    fprintf(stderr, "%s\n", error_buf);
    error_str = error_buf;
  }
}

// ================================================================
// load helpers
// ================================================================

void Storage::load_instruments() {
  fprintf(stderr, "load_instruments\n"); // DEBUG
  sqlite3_stmt *stmt;
  const char * const sql =
    "select id, type, name, port_name from instruments order by name, port_name";

  sqlite3_prepare_v3(db, sql, -1, 0, &stmt, nullptr);
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    int id = sqlite3_column_int(stmt, 0);
    int type = sqlite3_column_int(stmt, 1);
    const char *name = (const char *)sqlite3_column_text(stmt, 2);
    const char *port_name = (const char *)sqlite3_column_text(stmt, 3);
    PmDeviceID device_id = find_device(port_name, type);
    if (type == 0)
      pm->inputs.push_back(new Input(id, name, port_name, device_id));
    else
      pm->outputs.push_back(new Output(id, name, port_name, device_id));
  }
  sqlite3_finalize(stmt);
}

void Storage::load_messages() {
  fprintf(stderr, "load_messages\n"); // DEBUG
  sqlite3_stmt *stmt;
  const char * const sql = "select id, name, bytes from messages order by id";

  sqlite3_prepare_v3(db, sql, -1, 0, &stmt, nullptr);
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    int id = sqlite3_column_int(stmt, 0);
    const char *name = TEXT_OR_NULL(1, "");
    const char *bytes = (const char *)sqlite3_column_text(stmt, 2);

    Message *m = new Message(id, name);
    pm->messages.push_back(m);

    char *p = (char *)bytes;
    while (*p) {
      m->messages.push_back(pm_message_from_bytes(p));
      p += 6;
    }
  }
  sqlite3_finalize(stmt);
}

void Storage::load_triggers() {
  fprintf(stderr, "load_triggers\n"); // DEBUG
  sqlite3_stmt *stmt;
  const char * const sql =
"select id, input_id, trigger_message_bytes, action, message_id"
" from triggers"
" order by id";

  sqlite3_prepare_v3(db, sql, -1, 0, &stmt, nullptr);
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    int id = sqlite3_column_int(stmt, 0);
    int input_id = sqlite3_column_int(stmt, 1);
    const char *bytes = (const char *)sqlite3_column_text(stmt, 2);
    const char *action_name = (const char *)sqlite3_column_text(stmt, 3);
    int message_id = INT_OR_NULL(4, UNDEFINED);

    PmMessage trigger_message = pm_message_from_bytes((char *)bytes);
    Message *output_message = nullptr;
    if (message_id != UNDEFINED)
      output_message = find_message_by_id("trigger", id, message_id);

    TriggerAction action = MESSAGE;
    if (action_name == nullptr)
      action = MESSAGE;
    else if (strcmp(action_name, "next_song") == 0)
      action = NEXT_SONG;
    else if (strcmp(action_name, "prev_song") == 0)
      action = PREV_SONG;
    else if (strcmp(action_name, "next_patch") == 0)
      action = NEXT_PATCH;
    else if (strcmp(action_name, "prev_patch") == 0)
      action = PREV_PATCH;

    Trigger *t = new Trigger(id, trigger_message, action, output_message);
    Input *input = find_input_by_id("trigger", id, input_id);
    input->triggers.push_back(t);
  }
  sqlite3_finalize(stmt);
}

void Storage::load_songs() {
  fprintf(stderr, "load_songs\n"); // DEBUG
  sqlite3_stmt *stmt;
  const char * const sql = "select id, name, notes from songs order by name";

  sqlite3_prepare_v3(db, sql, -1, 0, &stmt, nullptr);
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    int id = sqlite3_column_int(stmt, 0);
    const char *name = (const char *)sqlite3_column_text(stmt, 1);
    const char *notes = (const char *)sqlite3_column_text(stmt, 2);

    Song *s = new Song(id, name);
    if (notes != nullptr) s->notes = notes;
    pm->all_songs->songs.push_back(s);
    load_patches(s);
  }
  sqlite3_finalize(stmt);
}

void Storage::load_patches(Song *s) {
  fprintf(stderr, "load_patches\n"); // DEBUG
  sqlite3_stmt *stmt;
  const char * const sql = "select id, name, start_message_id, stop_message_id from patches where song_id = ? order by position";

  sqlite3_prepare_v3(db, sql, -1, 0, &stmt, nullptr);
  sqlite3_bind_int(stmt, 1, s->id());
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    int id = sqlite3_column_int(stmt, 0);
    const char *name = (const char *)sqlite3_column_text(stmt, 1);
    int start_message_id = INT_OR_NULL(2, UNDEFINED);
    int stop_message_id = INT_OR_NULL(3, UNDEFINED);

    Patch *p = new Patch(id, name);
    if (id > max_patch_id)
      max_patch_id = id;

    if (start_message_id != UNDEFINED) {
      for (auto& message : pm->messages) {
        if (message->id() == start_message_id)
          p->start_message = message;
      }
      if (p->start_message == nullptr) {
        char error_buf[BUFSIZ];
        sprintf(error_buf,
                "patch %d (%s) can't find stop message with id %d\n",
                id, name, start_message_id);
        error_str = error_buf;
      }
    }
    if (stop_message_id != UNDEFINED) {
      for (auto& message : pm->messages) {
        if (message->id() == stop_message_id)
          p->stop_message = message;
      }
      if (p->stop_message == nullptr) {
        char error_buf[BUFSIZ];
        sprintf(error_buf,
                "patch %d (%s) can't find stop message with id %d\n",
                id, name, stop_message_id);
        error_str = error_buf;
      }
    }

    s->patches.push_back(p);
    load_connections(p);
  }

  sqlite3_finalize(stmt);
}

void Storage::create_default_patches() {
  fprintf(stderr, "create_default_patches\n"); // DEBUG
  for (auto& song : pm->all_songs->songs)
    if (song->patches.empty())
      create_default_patch(song);
}

void Storage::create_default_patch(Song *s) {
  fprintf(stderr, "create_default_patch\n"); // DEBUG
  Patch *p = new Patch(++max_patch_id, "Default Patch");
  s->patches.push_back(p);
  for (auto& input : pm->inputs) {
    for (auto& output : pm->outputs) {
      if (output->port_name == input->port_name) {
        Connection *conn =
          new Connection(++max_conn_id, input, CONNECTION_ALL_CHANNELS,
                         output, CONNECTION_ALL_CHANNELS);
        p->connections.push_back(conn);
      }
    }
  }
}

void Storage::load_connections(Patch *p) {
  fprintf(stderr, "load_connections\n"); // DEBUG
  sqlite3_stmt *stmt;
  const char * const sql =
"select id,"
"   input_id, input_chan, output_id, output_chan,"
"   bank_msb, bank_lsb, prog,"
"   zone_low, zone_high, xpose, pass_through_sysex"
" from connections"
" where patch_id = ?"
" order by position";

  sqlite3_prepare_v3(db, sql, -1, 0, &stmt, nullptr);
  sqlite3_bind_int(stmt, 1, p->id());
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    int id = sqlite3_column_int(stmt, 0);
    int input_id = sqlite3_column_int(stmt, 1);
    int input_chan = INT_OR_NULL(2, CONNECTION_ALL_CHANNELS);
    int output_id = sqlite3_column_int(stmt, 3);
    int output_chan = INT_OR_NULL(4, CONNECTION_ALL_CHANNELS);
    int bank_msb = INT_OR_NULL(5, UNDEFINED);
    int bank_lsb = INT_OR_NULL(6, UNDEFINED);
    int prog = INT_OR_NULL(7, UNDEFINED);
    int zone_low = INT_OR_NULL(8, 0);
    int zone_high = INT_OR_NULL(9, 127);
    int xpose = INT_OR_NULL(10, 0);
    int pass_through_sysex_bool = sqlite3_column_int(stmt, 11);

    Input *input = find_input_by_id("connection", id, input_id);
    Output *output = find_output_by_id("connection", id, output_id);
    Connection *conn = new Connection(id, input, input_chan, output, output_chan);
    if (id > max_conn_id)
      max_conn_id = id;
    conn->prog.bank_msb = bank_msb;
    conn->prog.bank_lsb = bank_lsb;
    conn->prog.prog = prog;
    conn->zone.low = zone_low;
    conn->zone.high = zone_high;
    conn->xpose = xpose;
    conn->pass_through_sysex = pass_through_sysex_bool != 0;

    load_controller_mappings(conn);

    p->connections.push_back(conn);
  }
  sqlite3_finalize(stmt);
}

void Storage::load_controller_mappings(Connection *conn) {
  fprintf(stderr, "load_controller_mappings\n"); // DEBUG
  sqlite3_stmt *stmt;
  const char * const sql =
"select id, cc_num, translated_cc_num, min, max, filtered"
" from controller_mappings"
" where connection_id = ?";

  sqlite3_prepare_v3(db, sql, -1, 0, &stmt, nullptr);
  sqlite3_bind_int(stmt, 1, conn->id());
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    int id = sqlite3_column_int(stmt, 0);
    int cc_num = sqlite3_column_int(stmt, 1);
    int translated_cc_num = sqlite3_column_int(stmt, 2);
    int min = sqlite3_column_int(stmt, 3);
    int max = sqlite3_column_int(stmt, 4);
    int filtered_bool = sqlite3_column_int(stmt, 5);

    Controller *cc = new Controller(id, cc_num);
    cc->translated_cc_num = translated_cc_num;
    cc->min = min;
    cc->max = max;
    cc->filtered = filtered_bool != 0;
    conn->cc_maps[cc->cc_num] = cc;
  }
  sqlite3_finalize(stmt);
}

void Storage::load_set_lists() {
  fprintf(stderr, "load_set_lists\n"); // DEBUG
  sqlite3_stmt *stmt;
  const char * const sql = "select id, name from set_lists order by name";

  sqlite3_prepare_v3(db, sql, -1, 0, &stmt, nullptr);
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    int id = sqlite3_column_int(stmt, 0);
    const char *name = (const char *)sqlite3_column_text(stmt, 1);
    SetList *slist = new SetList(id, name);
    pm->set_lists.push_back(slist);
    load_set_list_songs(slist);
  }
  sqlite3_finalize(stmt);
}

void Storage::load_set_list_songs(SetList *slist) {
  fprintf(stderr, "load_set_list_songs\n"); // DEBUG
  sqlite3_stmt *stmt;
  const char * const sql = "select song_id from set_lists_songs where set_list_id = ? order by position";

  sqlite3_prepare_v3(db, sql, -1, 0, &stmt, nullptr);
  sqlite3_bind_int(stmt, 1, slist->id());
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    int song_id = sqlite3_column_int(stmt, 0);
    Song *song = find_song_by_id("set list", slist->id(), song_id);
    slist->songs.push_back(song);
  }
  sqlite3_finalize(stmt);
}

// ================================================================
// save helpers
// ================================================================

void Storage::save_instruments() {
  sqlite3_stmt *stmt;
  const char * const sql =
    "insert into instruments (id, type, name, port_name) values (?, ?, ?, ?)";

  sqlite3_prepare_v3(db, sql, -1, 0, &stmt, nullptr);
  for (auto& input : pm->inputs) {
    sqlite3_bind_int(stmt, 1, input->id());
    sqlite3_bind_int(stmt, 2, 0);
    sqlite3_bind_text(stmt, 3, input->name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, input->port_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_reset(stmt);
  }
  for (auto& output : pm->outputs) {
    sqlite3_bind_int(stmt, 1, output->id());
    sqlite3_bind_int(stmt, 2, 1);
    sqlite3_bind_text(stmt, 3, output->name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, output->port_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_reset(stmt);
  }
  sqlite3_finalize(stmt);
}

void Storage::save_messages() {
  sqlite3_stmt *stmt;
  const char * const sql =
    "insert into messages (id, name, bytes) values (?, ?, ?)";

  sqlite3_prepare_v3(db, sql, -1, 0, &stmt, nullptr);
  for (auto& msg : pm->messages)
    save_message(stmt, msg);
  for (auto& song : pm->all_songs->songs)
    for (auto& patch : song->patches) {
      if (patch->start_message != nullptr)
        save_message(stmt, patch->start_message);
      if (patch->stop_message != nullptr)
        save_message(stmt, patch->stop_message);
    }
  sqlite3_finalize(stmt);
}

void Storage::save_message(sqlite3_stmt *stmt, Message *msg) {
  sqlite3_bind_int(stmt, 1, msg->id());
  sqlite3_bind_text(stmt, 2, msg->name.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 3, message_to_byte_str(msg).c_str(), -1, SQLITE_STATIC);
  sqlite3_step(stmt);
  sqlite3_reset(stmt);
}

void Storage::save_triggers() {
  sqlite3_stmt *stmt;
  const char * const sql =
    "insert into triggers (id, input_id, trigger_message_bytes, action, message_id) values (?, ?, ?, ?, ?)";

  sqlite3_prepare_v3(db, sql, -1, 0, &stmt, nullptr);
  for (auto& input : pm->inputs) {
    for (auto& trigger : input->triggers) {
      sqlite3_bind_int(stmt, 1, trigger->id());
      sqlite3_bind_int(stmt, 2, input->id());
      sqlite3_bind_text(stmt, 3, pm_message_to_bytes(trigger->trigger_message).c_str(),
                        -1, SQLITE_STATIC);
      if (trigger->output_message != nullptr) {
        sqlite3_bind_null(stmt, 4);
        sqlite3_bind_int(stmt, 5, trigger->output_message->id());
      }
      else {
        const char * action;
        switch (trigger->action) {
        case NEXT_SONG: action = "next_song"; break;
        case PREV_SONG: action = "prev_song"; break;
        case NEXT_PATCH: action = "next_patch"; break;
        case PREV_PATCH: action = "prev_patch"; break;
        default: break;
        }
        sqlite3_bind_text(stmt, 4, action, -1, SQLITE_STATIC);
        sqlite3_bind_null(stmt, 5);
      }
      sqlite3_step(stmt);
      sqlite3_reset(stmt);
    }
  }
  sqlite3_finalize(stmt);
}

void Storage::save_songs() {
  sqlite3_stmt *stmt;
  const char * const sql =
    "insert into songs (id, name, notes) values (?, ?, ?)";

  sqlite3_prepare_v3(db, sql, -1, 0, &stmt, nullptr);
  for (auto& song : pm->all_songs->songs) {
    sqlite3_bind_int(stmt, 1, song->id());
    sqlite3_bind_text(stmt, 2, song->name.c_str(), -1, SQLITE_STATIC);
    string notes = "";
    for (auto& line : song->notes)
      notes += line;
    if (notes == "")
      sqlite3_bind_null(stmt, 3);
    else
      sqlite3_bind_text(stmt, 3, notes.c_str(), -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_reset(stmt);
    save_patches(song);
  }
  sqlite3_finalize(stmt);
}

void Storage::save_patches(Song *song) {
  sqlite3_stmt *stmt;
  const char * const sql =
    "insert into patches (id, song_id, position, name, start_message_id, stop_message_id) values (?, ?, ?, ?, ?, ?)";

  sqlite3_prepare_v3(db, sql, -1, 0, &stmt, nullptr);
  int position = 0;
  for (auto& patch : song->patches) {
    sqlite3_bind_int(stmt, 1, patch->id());
    sqlite3_bind_int(stmt, 2, song->id());
    sqlite3_bind_int(stmt, 3, position++);
    sqlite3_bind_text(stmt, 4, patch->name.c_str(), -1, SQLITE_STATIC);
    BIND_ID_OR_NULL(5, patch->start_message);
    BIND_ID_OR_NULL(6, patch->stop_message);
    sqlite3_step(stmt);
    sqlite3_reset(stmt);
    save_connections(patch);
  }
  sqlite3_finalize(stmt);
}

void Storage::save_connections(Patch *patch) {
  sqlite3_stmt *stmt;
  const char * const sql =
    "insert into connections (id, patch_id, position, input_id, input_chan, output_id, output_chan, bank_msb, bank_lsb, prog, zone_low, zone_high, xpose, pass_through_sysex) values (?, ?, ?, ?, ?, ?)";

  sqlite3_prepare_v3(db, sql, -1, 0, &stmt, nullptr);
  int position = 0;
  for (auto& conn : patch->connections) {
    sqlite3_bind_int(stmt, 1, conn->id());
    sqlite3_bind_int(stmt, 2, patch->id());
    sqlite3_bind_int(stmt, 3, position++);
    sqlite3_bind_int(stmt, 4, conn->input->id());
    if (conn->input_chan == CONNECTION_ALL_CHANNELS)
      sqlite3_bind_null(stmt, 5);
    else
      sqlite3_bind_int(stmt, 5, conn->input_chan);
    sqlite3_bind_int(stmt, 6, conn->output->id());
    if (conn->output_chan == CONNECTION_ALL_CHANNELS)
      sqlite3_bind_null(stmt, 7);
    else
      sqlite3_bind_int(stmt, 7, conn->output_chan);
    BIND_INT_OR_NULL(8, conn->prog.bank_msb, UNDEFINED);
    BIND_INT_OR_NULL(9, conn->prog.bank_lsb, UNDEFINED);
    BIND_INT_OR_NULL(10, conn->prog.prog, UNDEFINED);
    sqlite3_bind_int(stmt, 11, conn->zone.low);
    sqlite3_bind_int(stmt, 12, conn->zone.high);
    sqlite3_bind_int(stmt, 13, conn->xpose);
    sqlite3_bind_int(stmt, 14, conn->pass_through_sysex ? 1 : 0);
    sqlite3_step(stmt);
    sqlite3_reset(stmt);
    save_controller_mappings(conn);
  }
  sqlite3_finalize(stmt);
}

void Storage::save_controller_mappings(Connection *conn) {
  sqlite3_stmt *stmt;
  const char * const sql =
    "insert into controller_mappings (id, connection_id, cc_num, translated_cc_num, min, max, filtered) values (?, ?, ?, ?, ?, ?, ?)";

  sqlite3_prepare_v3(db, sql, -1, 0, &stmt, nullptr);
  for (int i = 0; i < 128; ++i) {
    Controller *cc = conn->cc_maps[i];
    if (cc == nullptr)
      continue;

    sqlite3_bind_int(stmt, 1, cc->id());
    sqlite3_bind_int(stmt, 2, conn->id());
    sqlite3_bind_int(stmt, 3, cc->cc_num);
    sqlite3_bind_int(stmt, 4, cc->translated_cc_num);
    sqlite3_bind_int(stmt, 5, cc->min);
    sqlite3_bind_int(stmt, 6, cc->max);
    sqlite3_bind_int(stmt, 7, cc->filtered ? 1 : 0);
    sqlite3_step(stmt);
    sqlite3_reset(stmt);
  }
  sqlite3_finalize(stmt);
}

void Storage::save_set_lists() {
  sqlite3_stmt *stmt;
  const char * const sql =
    "insert into set_lists (id, name) values (?, ?)";

  sqlite3_prepare_v3(db, sql, -1, 0, &stmt, nullptr);
  for (vector<SetList *>::iterator iter = ++(pm->set_lists.begin());
       iter != pm->set_lists.end();
       ++iter)
  {
    SetList *set_list = *iter;
    sqlite3_bind_int(stmt, 1, set_list->id());
    sqlite3_bind_text(stmt, 2, set_list->name.c_str(), -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_reset(stmt);
    save_set_list_songs(set_list);
  }
  sqlite3_finalize(stmt);
}

void Storage::save_set_list_songs(SetList *set_list) {
  sqlite3_stmt *stmt;
  const char * const sql =
    "insert into set_lists_songs (set_list_id, song_id, position) values (?, ?, ?)";

  sqlite3_prepare_v3(db, sql, -1, 0, &stmt, nullptr);
  int position = 0;
  for (auto &song : set_list->songs) {
    sqlite3_bind_int(stmt, 1, set_list->id());
    sqlite3_bind_int(stmt, 2, song->id());
    sqlite3_bind_int(stmt, 3, position++);
    sqlite3_step(stmt);
    sqlite3_reset(stmt);
  }
  sqlite3_finalize(stmt);
}

PmDeviceID Storage::find_device(const char *name, int device_type) {
  if (pm->testing)
    return pmNoDevice;

  int num_devices = Pm_CountDevices();
  for (int i = 0; i < num_devices; ++i) {
    const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
    if (device_type == 0 && info->input && compare_device_names(name, (const char *)info->name) == 0)
      return i;
    if (device_type == 1 && info->output && compare_device_names(name, (const char *)info->name) == 0)
      return i;
  }
  return pmNoDevice;

}

// ================================================================
// find by id
// ================================================================

Input *Storage::find_input_by_id(
  const char * const searcher_name, int searcher_id, int id
) {
  for (auto &input : pm->inputs)
    if (input->id() == id)
      return input;
  set_find_error_message(searcher_name, searcher_id, "input", id);
  return nullptr;
}

Output *Storage::find_output_by_id(
  const char * const searcher_name, int searcher_id, int id
) {
  for (auto &output : pm->outputs)
    if (output->id() == id)
      return output;
  set_find_error_message(searcher_name, searcher_id, "output", id);
  return nullptr;
}

Message *Storage::find_message_by_id(
  const char * const searcher_name, int searcher_id, int id
) {
  for (auto &msg : pm->messages)
    if (msg->id() == id)
      return msg;
  set_find_error_message(searcher_name, searcher_id, "message", id);
  return nullptr;
}

Song *Storage::find_song_by_id(
  const char * const searcher_name, int searcher_id, int id
) {
  for (auto &song : pm->all_songs->songs)
    if (song->id() == id)
        return song;
  set_find_error_message(searcher_name, searcher_id, "song", id);
  return nullptr;
}

void Storage::set_find_error_message(
  const char * const searcher_name, int searcher_id,
  const char * const find_name, int find_id
) {
  char error_buf[BUFSIZ];
  sprintf(error_buf, "%s (%id) can't find %s with id %d\n",
          searcher_name, searcher_id, find_name, find_id);
  error_str = error_buf;
}

// ================================================================

/*
 * Case-insensitive string comparison that also ignores leading and trailing
 * whitespace. Assumes both names are shorter than BUFSIZ. Returns 0 if the
 * two strings are equal, given those conditions. Assumes both strings are
 * non-NULL.
 */
int Storage::compare_device_names(const char *name1, const char *name2) {
  while (isspace(*name1)) ++name1;
  while (isspace(*name2)) ++name2;
  if (*name1 == '\0' || *name2 == '\0')
    return *name1 - *name2;

  const char *end1 = name1 + strlen(name1) - 1;
  while(end1 > name1 && isspace(*end1)) end1--;
  const char *end2 = name2 + strlen(name2) - 2;
  while(end2 > name2 && isspace(*end2)) end2--;

  int len1 = (int)(end1 - name1) + 1;
  int len2 = (int)(end2 - name2) + 1;

  return strncasecmp(name1, name2, min(len1, len2));
}

PmMessage Storage::pm_message_from_bytes(char *bytes) {
  unsigned char b1 = 0, b2 = 0, b3 = 0;
  b1 = hex_to_byte(bytes); bytes += 2;
  if (*bytes) {
    b2 = hex_to_byte(bytes); bytes += 2;
    if (*bytes)
      b3 = hex_to_byte(bytes); bytes += 2;
  }
  return Pm_Message(b1, b2, b3);
}

string Storage::message_to_byte_str(Message *msg) {
  string byte_str;
  for (auto& pm_msg : msg->messages) {
    byte_str += pm_message_to_bytes((PmMessage)pm_msg);
  }
  return byte_str;
}

string Storage::pm_message_to_bytes(PmMessage msg) {
  char buf[7], *p = buf;

  sprintf(p, "%02x", Pm_MessageStatus(msg)); p += 2;
  sprintf(p, "%02x", Pm_MessageData1(msg)); p += 2;
  sprintf(p, "%02x", Pm_MessageData2(msg)); p += 2;
  *p = '\0';
  return string(buf);
}
