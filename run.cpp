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
  for (std::vector<fill>::const_iterator it = _fills.begin();
       it != _fills.end(); ++it) {
    r.before_cuts.insert(std::make_pair(it->name, it->before_cuts));
    r.after_cuts.insert(std::make_pair(it->name, it->after_cuts));
  }
  return r;
}

void run::process_event(const event &evt)
{
  for (std::vector<fill>::iterator it = _fills.begin();
       it != _fills.end(); ++it) {
    it->before_cuts.bin(it->function(evt));
  }
  for (std::vector<cut>::const_iterator it = _cuts.cbegin();
       it != _cuts.end(); ++it) {
    if (!it->function(evt)) {
      return;
    }
  }
  for (std::vector<fill>::iterator it = _fills.begin();
       it != _fills.end(); ++it) {
    it->after_cuts.bin(it->function(evt));
  }
}
