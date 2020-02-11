#include <pthread.h>
#include "midi_monitor.h"
#include "input.h"
#include "output.h"

class AsyncCallArgs {
public:
  AsyncCallArgs(AsyncMIDIMonitor *m, Instrument *i, PmMessage msg)
    : monitor(m), instrument(i), message(msg) {}

  AsyncMIDIMonitor *monitor;
  Instrument *instrument;
  PmMessage message;
};

void *async_monitor_input(void *vptr) {
  AsyncCallArgs *args = (AsyncCallArgs *)vptr;
  args->monitor->do_monitor_input((Input *)args->instrument, args->message);
  delete args;
  return nullptr;
}

void *async_monitor_output(void *vptr) {
  AsyncCallArgs *args = (AsyncCallArgs *)vptr;
  args->monitor->do_monitor_output((Output *)args->instrument, args->message);
  delete args;
  return nullptr;
}

void AsyncMIDIMonitor::monitor_input(Input *input, PmMessage msg) {
  pthread_t thread;
  AsyncCallArgs *args = new AsyncCallArgs(this, input, msg);
  pthread_create(&thread, nullptr, async_monitor_input, (void *)args);
}

void AsyncMIDIMonitor::monitor_output(Output *output, PmMessage msg) {
  pthread_t thread;
  AsyncCallArgs *args = new AsyncCallArgs(this, output, msg);
  pthread_create(&thread, nullptr, async_monitor_output, (void *)args);
}
