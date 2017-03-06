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

  program last_program_change_seen(int chan) { return seen_progs[chan]; }

private:
  pthread_t portmidi_thread;
  List<Connection *> notes_off_conns[MIDI_CHANNELS][NOTES_PER_CHANNEL];
  List<Connection *> sustain_off_conns[MIDI_CHANNELS];
  program seen_progs[MIDI_CHANNELS];

  void remember_program_change_messages(PmMessage msg);
  List<Connection *> &connections_for_message(PmMessage msg);
};

#endif /* INPUT_H */
