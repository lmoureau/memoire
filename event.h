#ifndef EVENT_H
#define EVENT_H

#include <vector>

#include "castor.h"
#include "lorentz.h"

struct track
{
  int pdgid; ///< PDG
  int gpid;  ///< GEANT
  lorentz::vec p;
  int charge;
  int matched = -1;
  double chi2;
  int ndof;
  lorentz::vec x; ///< Temporal component == 0!

  void match(const std::vector<track> &tracks);
};

struct plus_minus
{
  double plus, minus;
};

struct barrel_endcap
{
  plus_minus barrel, endcap;
};

struct hadronic_calorimeter : public barrel_endcap
{
  plus_minus forward;
};

struct event
{
  int id;
  std::vector<track> tracks;
  lorentz::vec p;
  castor castor_status;
  hadronic_calorimeter hcal;
  barrel_endcap ecal;

  void add_track(const track &t);
};

#endif // EVENT_H
