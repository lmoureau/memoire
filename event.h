#ifndef EVENT_H
#define EVENT_H

#include <vector>

#include "lorentz.h"

struct track
{
  int pdgid; ///< PDG
  int gpid;  ///< GEANT
  lorentz::vec p;
};

struct event
{
  int id;
  std::vector<track> tracks;
};

#endif // EVENT_H
