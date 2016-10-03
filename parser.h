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
public:
  explicit starlight_parser(const std::string &filename);

  bool end() override;
  event next() override;
  void reset() override;
};

#endif // PARSER_H
