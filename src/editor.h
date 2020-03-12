#ifndef EDITOR_H
#define EDITOR_H

#include "patchmaster.h"

class Editor {
public:
  Editor(PatchMaster *pm = nullptr); // defaults to PatchMaster_instance()

  Message *create_message();
  Trigger *create_trigger(Input *input);
  Song *create_song();
  Patch *create_patch();
  Patch *create_patch(Song *song);
  Connection *create_connection(Patch *patch, Input *input, Output *output);
  SetList * create_set_list();

  bool ok_to_destroy_message(Message *message);
  bool ok_to_destroy_trigger(Trigger *trigger);
  bool ok_to_destroy_song(Song *song);
  bool ok_to_destroy_patch(Song *song, Patch *patch);
  bool ok_to_destroy_connection(Patch *patch, Connection *connection);
  bool ok_to_destroy_set_list(SetList *set_list);

  void destroy_message(Message *message);
  void destroy_trigger(Trigger *trigger);
  void destroy_song(Song *song);
  void destroy_patch(Song *song, Patch *patch);
  void destroy_connection(Patch *patch, Connection *connection);
  void destroy_set_list(SetList *set_list);

  void remove_song_from_set_list(Song *song, SetList *set_list);

  void move_away_from_song(Song *);
  void move_away_from_patch(Song *, Patch *);

private:
  PatchMaster *pm;
};

#endif /* EDITOR_H */
