#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <memory>
#include <string>

#include "event.h"
#include "event_source.h"

class starlight_parser : public event_source
{
  std::ifstream _in;
  std::string _filename;
  event _current;

  starlight_parser(const starlight_parser &) {}

public:
  explicit starlight_parser(const std::string &filename);

  bool end();
  void read();
  const event &rec();
  const event &gen();
  void reset();
};

class root_parser : public event_source
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
  const event &rec();
  const event &gen();
  void reset();
};

class hlt_parser : public event_source
{
  std::string _filename;
  struct data;
  std::shared_ptr<data> _d;

private:
  hlt_parser(const hlt_parser &) {}

public:
  explicit hlt_parser(const std::string &filename);

  bool end();
  void read();
  void prepare(sol::state &lua, sol::table &event);
  void fill_rec(sol::state &lua, sol::table &event);
  const event &rec();
  const event &gen();
  void reset();
};

#endif // PARSER_H
