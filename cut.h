#ifndef CUT_H
#define CUT_H

#include <string>

class event;

class cut
{
  std::string _name;
public:
  explicit cut(const std::string &name) : _name(name) {}
  virtual ~cut() {}

  virtual bool operator() (const event &e) = 0;
};

class lambda_cut : public cut
{
public:
  using lambda_function = bool (*) (const event &e);

private:
  lambda_function _fct;

public:
  explicit lambda_cut(const std::string &name, lambda_function fct) :
    cut(name), _fct(fct) {}

  virtual bool operator() (const event &e) { return _fct(e); }
};

#include <string>

#include "sol.hpp"

#include "event.h"

class lua_cut : public cut
{
  sol::state _lua;
  std::string _code;
public:
  explicit lua_cut(const std::string &name, const std::string &code) :
    cut(name),
    _code("cut_result=" + code) {}

  virtual bool operator() (const event &e)
  {
    _lua["p"] = _lua.create_table_with("t", e.p.t(), "x", e.p.x(),
                                       "y", e.p.y(), "z", e.p.z());
    _lua.script(_code);
    return _lua["cut_result"];
  }
};

#endif // CUT_H
