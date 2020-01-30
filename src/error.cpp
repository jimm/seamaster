#include "error.h"
#include "wx/app.h"

void error_message(const char * const msg) {
  fprintf(stderr, "%s\n", msg);
  App *app = app_instance();
  if (app != nullptr) {
    app->show_message(msg);
    app->clear_message_after(10);
  }
}
