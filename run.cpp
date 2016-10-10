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
      name,
      fct,
      hist::histogram(axis),
      hist::histogram(axis),
      hist::histogram2d(axis, axis) });
}

run::result run::operator() (parser *in)
{
  for (in->read(); !in->end(); in->read()) {
    process_event(in);
  }
  result r;
  for (std::vector<fill>::const_iterator it = _fills.begin();
       it != _fills.end(); ++it) {
    item i = item {
      it->after_cuts,
      it->before_cuts,
      it->migration
    };
    r.histos.insert(std::make_pair(it->name, i));
  }
  return r;
}

void run::process_event(parser *in)
{
  bool has_gen = in->has_gen();
  event gen = has_gen ? in->gen() : event();
  bool has_rec = in->has_rec();
  event rec = has_rec ? in->rec() : event();

  bool passes_cuts = true;
  if (has_rec) {
    for (const cut &c : _cuts) {
      if (!c.function(rec)) {
        passes_cuts = false;
        break;
      }
    }
  }

  for (fill &f : _fills) {
    double gen_val = 0, rec_val = 0;
    if (has_gen) {
      gen_val = f.function(gen);
      f.before_cuts.bin(gen_val);
    }
    if (has_rec && passes_cuts) {
      rec_val = f.function(rec);
      f.after_cuts.bin(rec_val);
    }
    if (has_gen && has_rec && passes_cuts) {
      f.migration.bin(hist::bin2d(gen_val, rec_val));
      // TODO fake, miss
    }
  }
}
