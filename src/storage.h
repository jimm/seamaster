#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include <vector>
#include <sqlite3.h>
#include "patchmaster.h"

using namespace std;

class Storage {
public:
  Storage(const char *path);
  ~Storage();

  void initialize();
  void initialize_with_schema_file(string config_path); // public for testing only
  PatchMaster *load(bool testing);
  void save(PatchMaster *pm);
  bool has_error();
  string error();

private:
  sqlite3 *db;
  PatchMaster *pm;
  string error_str;
  int max_patch_id;
  int max_conn_id;

  void load_instruments();
  void load_messages();
  void load_triggers();
  void load_songs();
  void load_patches(Song *);
  void load_connections(Patch *);
  void load_controller_mappings(Connection *);
  void load_set_lists();
  void load_set_list_songs(SetList *);

  void save_instruments();
  void save_messages();
  void save_message(sqlite3_stmt *, Message *);
  void save_triggers();
  void save_songs();
  void save_patches(Song *);
  void save_connections(Patch *);
  void save_controller_mappings(Connection *);
  void save_set_lists();
  void save_set_list_songs(SetList *);

  void create_default_patches();
  void create_default_patch(Song *);

  PmDeviceID find_device(const char *name, int device_type);
  Input *find_input_by_id(const char * const, int, int);
  Output *find_output_by_id(const char * const, int, int);
  Message *find_message_by_id(const char * const, int, int);
  Song *find_song_by_id(const char * const, int, int);
  void set_find_error_message(const char * const, int, const char * const, int);

  int compare_device_names(const char *name1, const char *name2);

  PmMessage pm_message_from_bytes(char *);
  string message_to_byte_str(Message *);
  string pm_message_to_bytes(PmMessage msg);
};

#endif /* STORAGE_H */
