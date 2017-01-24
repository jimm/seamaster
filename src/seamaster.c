#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "patchmaster.h"
#include "load.h"
#include "curses/gui.h"

void list_devices(const char *title, const PmDeviceInfo *infos[], int num_devices) {
  printf("%s:\n", title);
  for (int i = 0; i < num_devices; ++i)
    if (infos[i] != 0)
      printf("  %2d: %s%s\n", i, infos[i]->name, infos[i]->opened ? " (open)" : "");
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

int main(int argc, const char **argv) {
  Pm_Initialize();
  atexit(cleanup);

  if (argc > 1 && strncmp("-l", argv[1], 2) == 0)
    list_all_devices();
  else {
    patchmaster *pm = patchmaster_new();
    if (argc > 1) {
      if (load(pm, argv[1]) != 0)
        exit(1);                /* error already printed */
    }
    patchmaster_start(pm);
    gui_main(pm);
    patchmaster_stop(pm);
    patchmaster_free(pm);
  }

  exit(0);
  return 0;
}
