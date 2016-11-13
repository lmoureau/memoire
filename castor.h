#ifndef CASTOR_H
#define CASTOR_H

#include <vector>

class castor
{
  struct hit
  {
    int module;
    int sector;
    double data;
  };
  std::vector<hit> _hits;
  double _energy;

public:
  void add_hit(int module, int sector, double data);

  double energy() const { return _energy; }
};

#endif // CASTOR_H
