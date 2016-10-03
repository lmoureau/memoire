#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <string>

#include "event.h"

class parser
{
public:
  virtual ~parser() {}

  virtual bool end() = 0;
  virtual event next() = 0;
  virtual void reset() = 0;
};

class starlight_parser : public parser
{
  std::ifstream _in;
  std::string _filename;

private:
  starlight_parser(const starlight_parser &) {}

public:
  explicit starlight_parser(const std::string &filename);

  bool end();
  event next();
  void reset();
};

#endif // PARSER_H
