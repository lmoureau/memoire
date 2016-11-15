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

#endif // CUT_H
