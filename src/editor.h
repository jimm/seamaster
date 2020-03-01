#ifndef EDITOR_H
#define EDITOR_H

#include "patchmaster.h"

class Editor {
public:
  Editor(PatchMaster *pm = nullptr); // defaults to PatchMaster_instance()

  void create_message();
  void create_trigger(Input *input);
  void create_song();
  void create_song(SetList *set_list, int positino);
  void create_patch();
  void create_patch(Song *song);
  void create_connection(Input *input, Output *output);
  void create_set_list();

  void destroy_message(Message *message);
  void destroy_trigger(Trigger *trigger);
  void destroy_song(Song *song);
  void destroy_patch(Song *song, Patch *patch);
  void destroy_connection(Patch *patch, Connection *connection);
  void destroy_set_list(SetList *set_list);

  void remove_song_from_set_list(Song *song, SetList *set_list);

private:
  PatchMaster *pm;
};

#endif /* EDITOR_H */
