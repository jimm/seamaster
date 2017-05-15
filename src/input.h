#ifndef INPUT_H
#define INPUT_H

#include <pthread.h>
#include <portmidi.h>
#include "consts.h"
#include "instrument.h"
#include "connection.h"
#include "trigger.h"

using namespace std;

class Input : public Instrument {
public:
  vector<Connection *> connections;
  vector<Trigger *> triggers;
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

  program last_program_change_seen(int chan) { return seen_progs[chan]; }

private:
  pthread_t portmidi_thread;
  vector<Connection *> notes_off_conns[MIDI_CHANNELS][NOTES_PER_CHANNEL];
  vector<Connection *> sustain_off_conns[MIDI_CHANNELS];
  program seen_progs[MIDI_CHANNELS];

  void remember_program_change_messages(PmMessage msg);
  vector<Connection *> &connections_for_message(PmMessage msg);
};

#endif /* INPUT_H */
