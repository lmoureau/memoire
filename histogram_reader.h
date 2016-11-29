#ifndef MAKE_HIST_H
#define MAKE_HIST_H

#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "sol.hpp"

#include "histogram.h"

class histogram_reader
{
  sol::state _lua; // Keep this first!
  sol::table _data;
public:
  explicit histogram_reader(const std::string &filename);
  explicit histogram_reader(std::istream &in);

  std::vector<std::string> names() const;
  std::vector<std::pair<double, double>> data(const std::string &name) const;
  hist::histogram histogram(const std::string &name,
                            double min = std::numeric_limits<double>::lowest(),
                            double max = std::numeric_limits<double>::max(),
                            int nbins = 100) const;
};

#endif // MAKE_HIST_H
