#ifndef RUN_H
#define RUN_H

#include <functional>
#include <map>
#include <vector>

#include "event.h"
#include "histogram.h"
#include "parser.h"

class run
{
public:
  struct result
  {
    std::map<std::string, hist::histogram> after_cuts;
    std::map<std::string, hist::histogram> before_cuts;
  };

  typedef std::function<bool (const event &evt)> cut_fct;
  typedef std::function<double (const event &evt)> fill_fct;

private:
  struct cut
  {
    std::string name;
    cut_fct function;
  };

  struct fill
  {
    std::string name;
    fill_fct function;
    hist::histogram before_cuts;
    hist::histogram after_cuts;
  };

  std::vector<fill> _fills;
  std::vector<cut> _cuts;

public:
  explicit run() {}
  virtual ~run() {}

  void add_cut(const std::string &name, const cut_fct cut);
  void add_fill(const std::string &name, const hist::linear_axis<double> &axis,
                const fill_fct fill);

  result operator() (parser *in);

private:
  void process_gen_event(const event &evt);
  void process_rec_event(const event &evt);
};

#endif // RUN_H
