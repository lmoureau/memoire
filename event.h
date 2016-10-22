#ifndef EVENT_H
#define EVENT_H

#include <vector>

#include "lorentz.h"

struct track
{
  int pdgid; ///< PDG
  int gpid;  ///< GEANT
  lorentz::vec p;
  int matched = -1;

  void match(const std::vector<track> &tracks);
};

struct event
{
  int id;
  std::vector<track> tracks;
  lorentz::vec p;

  void add_track(const track &t);
};

#endif // EVENT_H
