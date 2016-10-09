#include "parser.h"

#include <cstdlib>
#include <string>

#include <TFile.h>
#include <TTree.h>

starlight_parser::starlight_parser(const std::string &filename) :
  _filename(filename),
  _in(filename)
{}

bool starlight_parser::end()
{
  char peek = _in.peek();
  while (peek == ' ' || peek == '\n') {
    _in.get();
    peek = _in.peek();
  }
  return _in.eof();
}

namespace {
  double read_double(std::istream &in)
  {
    std::string token;
    in >> token;
    return std::strtod(&token[0], 0);
  }
}

void starlight_parser::read()
{
  // Intermediate data
  std::string prefix;
  int ntracks;

  // Unused data
  int itrash;

  _current = event();

  _in >> prefix;
  if (prefix != "EVENT:") {
    throw 1;
  }
  _in >> _current.id;
  _in >> ntracks;
  _in >> itrash; // vertex count

  _in >> prefix;
  if (prefix != "VERTEX:") {
    throw 2;
  }
  read_double(_in); // x
  read_double(_in); // y
  read_double(_in); // z
  read_double(_in); // t
  _in >> itrash /* vertex number */ >> itrash /* physical process */
       >> itrash /* parent track */ >> itrash /* daughter track count */;

  for (int i = 0; i < ntracks; i++) {
    _in >> prefix;
    if (prefix != "TRACK:") {
      throw 3;
    }
    track trk;
    _in >> trk.gpid;
    double px = read_double(_in);
    double py = read_double(_in);
    double pz = read_double(_in);
    trk.p = lorentz::vec::mxyz(.14, px, py, pz);
    _in >> itrash /* event number */ >> itrash /* starting vertex */
         >> itrash /* ending vertex */;
    _in >> trk.pdgid;

    // PYTHIA fields; read until eol
    char line[1024];
    _in.getline(line, sizeof(line));

    _current.tracks.push_back(trk);
  }
}

event starlight_parser::gen()
{
  return _current;
}

event starlight_parser::rec()
{
  return _current;
}

void starlight_parser::reset()
{
  if (_in.is_open()) {
    _in.close();
  }
  _in.open(_filename);
}

struct root_parser::data
{
  TFile *file;
  TTree *gen_tree, *rec_tree;
  long gen_count, current_gen;
  long rec_count, current_rec;
  event gen, rec;

  int gen_i;
  double gen_pxp;
  double gen_pyp;
  double gen_pzp;
  double gen_pxm;
  double gen_pym;
  double gen_pzm;

  int rec_i;
  double rec_pxp;
  double rec_pyp;
  double rec_pzp;
  double rec_pxm;
  double rec_pym;
  double rec_pzm;
};

root_parser::root_parser(const std::string &filename) :
  _filename(filename),
  _d(new data)
{
  // Trees: rho_gen, rho_rec
  _d->file = new TFile(filename.c_str());
  _d->file->GetObject("rho_rec", _d->rec_tree);
  _d->file->GetObject("rho_gen", _d->gen_tree);
  _d->gen_count = _d->gen_tree->GetEntries();
  _d->rec_count = _d->rec_tree->GetEntries();
  reset();

  _d->gen_tree->SetBranchAddress("gen_i", &_d->gen_i);
  _d->gen_tree->SetBranchAddress("gen_pxp", &_d->gen_pxp);
  _d->gen_tree->SetBranchAddress("gen_pyp", &_d->gen_pyp);
  _d->gen_tree->SetBranchAddress("gen_pzp", &_d->gen_pzp);
  _d->gen_tree->SetBranchAddress("gen_pxm", &_d->gen_pxm);
  _d->gen_tree->SetBranchAddress("gen_pym", &_d->gen_pym);
  _d->gen_tree->SetBranchAddress("gen_pzm", &_d->gen_pzm);

  _d->rec_tree->SetBranchAddress("rec_i", &_d->rec_i);
  _d->rec_tree->SetBranchAddress("rec_pxp", &_d->rec_pxp);
  _d->rec_tree->SetBranchAddress("rec_pyp", &_d->rec_pyp);
  _d->rec_tree->SetBranchAddress("rec_pzp", &_d->rec_pzp);
  _d->rec_tree->SetBranchAddress("rec_pxm", &_d->rec_pxm);
  _d->rec_tree->SetBranchAddress("rec_pym", &_d->rec_pym);
  _d->rec_tree->SetBranchAddress("rec_pzm", &_d->rec_pzm);
}

bool root_parser::end()
{
  return _d->current_gen >= _d->gen_count;
}

void root_parser::read()
{
  _d->gen_tree->GetEntry(_d->current_gen++);
  _d->gen = event();

  track trk;
  trk.p = lorentz::vec::mxyz(.14, _d->gen_pxp, _d->gen_pyp, _d->gen_pzp);
  _d->gen.tracks.push_back(trk);
  trk.p = lorentz::vec::mxyz(.14, _d->gen_pxm, _d->gen_pym, _d->gen_pzm);
  _d->gen.tracks.push_back(trk);

  if (_d->rec_i < _d->gen_i) {
    _d->rec_tree->GetEntry(_d->current_rec++);
  }
  if (_d->rec_i == _d->gen_i) {
    _d->rec = event();

    trk.p = lorentz::vec::mxyz(.14, _d->rec_pxp, _d->rec_pyp, _d->rec_pzp);
    _d->rec.tracks.push_back(trk);
    trk.p = lorentz::vec::mxyz(.14, _d->rec_pxm, _d->rec_pym, _d->rec_pzm);
    _d->rec.tracks.push_back(trk);
  }
}

event root_parser::gen()
{
  return _d->gen;
}

bool root_parser::has_rec()
{
  return _d->rec_i == _d->gen_i;
}

event root_parser::rec()
{
  return _d->rec;
}

void root_parser::reset()
{
  _d->current_gen = 0;
  _d->gen_i = 0;
  _d->current_rec = 0;
  _d->rec_i = -1;
}
