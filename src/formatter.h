#ifndef FORMATTER_H
#define FORMATTER_H

void note_num_to_name(int num, char *buf);
int note_name_to_num(const char *str); // str may point to an integer string like "64" as well

#endif /* FORMATTER_H */