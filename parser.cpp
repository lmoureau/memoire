#include "parser.h"

#include <cstdlib>
#include <string>

#include <TFile.h>

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

event starlight_parser::next()
{
  event evt;

  // Intermediate data
  std::string prefix;
  int ntracks;

  // Unused data
  int itrash;

  _in >> prefix;
  if (prefix != "EVENT:") {
    throw 1;
  }
  _in >> evt.id;
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

    evt.tracks.push_back(trk);
  }

  return evt;
}

void starlight_parser::reset()
{
  if (_in.is_open()) {
    _in.close();
  }
  _in.open(_filename);
}

root_parser::root_parser(const std::string &filename) :
  _filename(filename),
  _in()
{
  TFile *file = new TFile(filename.c_str());
  file->ls();
  delete file;
}

bool root_parser::end()
{
  return false;
}

event root_parser::next()
{
  event evt;
  return evt;
}

void root_parser::reset()
{
}
