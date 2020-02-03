#ifndef FORMATTER_H
#define FORMATTER_H

#include "connection.h"

void note_num_to_name(int num, char *buf);
int note_name_to_num(const char *str); // str may point to an integer string like "64" as well
void format_program(program prog, char *buf);
void format_program_no_spaces(program prog, char *buf);
void format_controllers(Connection *conn, char *buf);
string byte_to_hex(unsigned char byte);

#endif /* FORMATTER_H */