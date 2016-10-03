#include "run.h"

void run::add_cut(const std::string &name, const cut_fct fct)
{
  _cuts.push_back(cut{ name, fct });
}

void run::add_fill(const std::string &name,
                   const hist::linear_axis<double> &axis,
                   const fill_fct fct)
{
  _fills.push_back(fill{
      name, fct, hist::histogram(axis), hist::histogram(axis) });
}

run::result run::operator() (parser *in)
{
  while (!in->end()) {
    process_event(in->next());
  }
  result r;
  for (const fill &f : _fills) {
    r.before_cuts.insert(std::make_pair(f.name, f.before_cuts));
    r.after_cuts.insert(std::make_pair(f.name, f.after_cuts));
  }
  return r;
}

void run::process_event(const event &evt)
{
  for (fill &f : _fills) {
    f.before_cuts.bin(f.function(evt));
  }
  for (const cut &c : _cuts) {
    if (!c.function(evt)) {
      return;
    }
  }
  for (fill &f : _fills) {
    f.after_cuts.bin(f.function(evt));
  }
}
