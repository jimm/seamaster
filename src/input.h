#ifndef INPUT_H
#define INPUT_H

#include <pthread.h>
#include <portmidi.h>
#include "instrument.h"
#include "consts.h"
#include "list.h"
#include "connection.h"
#include "trigger.h"

class Input : public Instrument {
public:
  list *connections;
  list *triggers;
  bool running;
  pthread_t portmidi_thread;
  list *notes_off_conns[MIDI_CHANNELS][NOTES_PER_CHANNEL];

  Input(const char *sym, const char *name, int port_num);
  ~Input();

  void add_connection(Connection *);
  void remove_connection(Connection *);

  void add_trigger(trigger *);
  void remove_trigger(trigger *);

  void start();
  void stop();

  void read(PmEvent *buf, int len);

  void debug();
};

#endif /* INPUT_H */
