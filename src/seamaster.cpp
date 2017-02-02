#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include "patchmaster.h"
#include "loader.h"
#include "curses/gui.h"

void list_devices(const char *title, const PmDeviceInfo *infos[], int num_devices) {
  printf("%s:\n", title);
  for (int i = 0; i < num_devices; ++i)
    if (infos[i] != 0) {
      const char *name = infos[i]->name;
      const char *q = (name[0] == ' ' || name[strlen(name)-1] == ' ') ? "\"" : "";
      printf("  %2d: %s%s%s%s\n", i, q, name, q, infos[i]->opened ? " (open)" : "");
    }
}

void list_all_devices() {
  int num_devices = Pm_CountDevices();
  const PmDeviceInfo *inputs[num_devices], *outputs[num_devices];

  for (int i = 0; i < num_devices; ++i) {
    const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
    inputs[i] = info->input ? info : 0;
    outputs[i] = info->output ? info : 0;
  }

  list_devices("Inputs", inputs, num_devices);
  list_devices("Outputs", outputs, num_devices);
}

void cleanup() {
  Pm_Terminate();
}

void usage(char *prog_name) {
  fprintf(stderr,
          "usage: %s [-l] [-n] [-?|-h] file\n"
          "\n"
          "    -l     List all attached MIDI ports\n"
          "    -n     No MIDI (ignores bad/unknown MIDI ports)\n"
          "    -?, -h This help\n",
          basename(prog_name));
}

int main(int argc, char * const *argv) {
  PatchMaster pm;
  int ch, testing = false;
  char *prog_name = argv[0];

  while ((ch = getopt(argc, argv, "lnh?")) != -1) {
    switch (ch) {
    case 'l':
      list_all_devices();
      exit(0);
    case 'n':
      testing = true;
      break;
    case '?': case 'h': default:
      usage(prog_name);
      exit(ch == '?' || ch == 'h' ? 0 : 1);
    }
  }
  argc -= optind;
  argv += optind;

  if (argc == 0) {
    usage(prog_name);
    exit(1);
  }

  Pm_Initialize();
  atexit(cleanup);

  pm.testing = testing;

  Loader loader(pm);
  if (loader.load(argv[0]) != 0)
    exit(1);                    // error already printed

  pm.start();
  GUI gui(pm);
  gui.run();
  pm.stop();

  exit(0);
  return 0;
}
