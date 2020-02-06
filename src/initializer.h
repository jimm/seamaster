#ifndef INITIALIZER_H
#define INITIALIZER_H

class PatchMaster;

class Initializer {
public:
  void initialize();

private:
  void load_instruments(PatchMaster *pm);
  void create_songs(PatchMaster *pm);
};

#endif /* INITIALIZER_H */
