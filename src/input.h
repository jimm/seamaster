#ifndef INPUT_H
#define INPUT_H

#include <pthread.h>
#include <portmidi.h>
#include "consts.h"
#include "list.h"
#include "instrument.h"
#include "connection.h"
#include "trigger.h"

class Input : public Instrument {
public:
  List<Connection *> connections;
  List<Trigger *> triggers;
  bool running;

  Input(const char *sym, const char *name, int port_num);
  ~Input();

  void add_connection(Connection *);
  void remove_connection(Connection *);

  void add_trigger(Trigger *);
  void remove_trigger(Trigger *);

  void start();
  void stop();

  void read(PmEvent *buf, int len);

  void debug();

private:
  pthread_t portmidi_thread;
  List<Connection *> notes_off_conns[MIDI_CHANNELS][NOTES_PER_CHANNEL];
  List<Connection *> sustain_off_conns[MIDI_CHANNELS];

  List<Connection *> &connections_for_message(PmMessage msg);
};

#endif /* INPUT_H */
