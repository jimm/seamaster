#ifndef PROGRAM_H
#define PROGRAM_H

typedef struct program {
  byte bank_msb;
  byte bank_lsb;
  byte pc_prog;
} program;

#endif  /* PROGRAM_H */
