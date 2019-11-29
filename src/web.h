#ifndef WEB_H
#define WEB_H

#include <stdio.h>
#include <string>
#include "patchmaster.h"
#include "named.h"

using namespace std;

class Web {
public:
  Web(PatchMaster *pm, int port_num);
  ~Web();

  int run();

private:
  PatchMaster *pm;
  int port_num;
  FILE *stream;
  int childfd;

  void error(const char *);
  void cerror(const char *cause, const char *error_number,
              const char *shortmsg, const char *longmsg);
  void return_status();
  void append_connection(string &, Connection *);
  void append_instrument_fields(string &, Instrument *);
  void append_json_list_of_names(string &, vector<Named *> &);
  void append_quoted_string(string &str, string &quote_me);
};

#endif /* WEB_H */
