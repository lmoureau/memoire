#include "run.h"

run::run() :
  _lua(std::make_shared<sol::state>())
{
  _lua->open_libraries(sol::lib::base,
                       sol::lib::math,
                       sol::lib::package,
                       sol::lib::table);
  std::string oldpath = (*_lua)["package"]["path"];
  (*_lua)["package"]["path"] = oldpath + ";./lua/?.lua;../lua/?.lua";
}

void run::add(const std::shared_ptr<cut> &c)
{
  _cuts.push_back(c);
}

void run::add(const std::shared_ptr<lua_cut> &c)
{
  sol::load_result result = c->load_into(*_lua);
  if (!result.valid()) {
    throw sol::error(result.get<std::string>());
  }
  _lua_cuts.push_back(result);
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

run::result run::operator() (event_source *in)
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

void run::process_event(event_source *in)
{
  in->prepare(*_lua);
  bool has_gen = in->has_gen();
  const event &gen = has_gen ? in->gen() : event();
  in->fill_rec(*_lua);
  bool has_rec = in->has_rec();
  const event &rec = has_rec ? in->rec() : event();

  bool passes_cuts = true;
  if (has_rec) {
    for (auto c : _cuts) {
      if (!(*c)(rec)) {
        passes_cuts = false;
        break;
      }
    }
    for (auto func : _lua_cuts) {
      if (!func()) {
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
