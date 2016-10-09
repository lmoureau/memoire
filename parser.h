#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <memory>
#include <string>

#include "event.h"

class parser
{
public:
  virtual ~parser() {}

  virtual bool end() = 0;
  virtual bool has_gen() { return true; }
  virtual bool has_rec() { return true; }
  virtual void read() = 0;
  virtual event rec() = 0;
  virtual event gen() = 0;
  virtual void reset() = 0;
};

class starlight_parser : public parser
{
  std::ifstream _in;
  std::string _filename;
  event _current;

  starlight_parser(const starlight_parser &) {}

public:
  explicit starlight_parser(const std::string &filename);

  bool end();
  void read();
  event rec();
  event gen();
  void reset();
};

class root_parser : public parser
{
  std::string _filename;
  struct data;
  std::shared_ptr<data> _d;

private:
  root_parser(const root_parser &) {}

public:
  explicit root_parser(const std::string &filename);

  bool end();
  bool has_rec();
  void read();
  event rec();
  event gen();
  void reset();
};

#endif // PARSER_H
