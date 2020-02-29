#ifndef EDITOR_H
#define EDITOR_H

#include "patchmaster.h"

class Editor {
public:
  Editor(PatchMaster *pm = nullptr); // defaults to PatchMaster_instance()

  void create_message();
  void create_trigger(Input *input);
  void create_song();
  void create_patch();
  void create_connection(Input *input, Output *output);
  void create_set_list();

  void destroy_message(Message *m);
  void destroy_trigger(Trigger *t);
  void destroy_song(Song *s);
  void destroy_patch(Song *s, Patch *p);
  void destroy_connection(Patch *p, Connection *c);
  void destroy_set_list(SetList *sl);

  void remove_song_from_set_list(Song *song, SetList *set_list);

private:
  PatchMaster *pm;
};

#endif /* EDITOR_H */
