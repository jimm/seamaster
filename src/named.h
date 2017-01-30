#ifndef NAMED_H
#define NAMED_H

class Named {
public:
  char *name;

  Named(const char *name);
  virtual ~Named();
};

#endif /* NAMED_H */
