#ifndef DB_OBJ_H
#define DB_OBJ_H

class DBObj {
public:
  DBObj(int i) : _id(i) {}
  virtual ~DBObj() {}

  int id() { return _id; }

private:
  int _id;
};

#endif /* DB_OBJ_H */