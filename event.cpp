#include "event.h"

#include <algorithm>

void track::match(const std::vector<track> &tracks)
{
  auto it = std::min_element(tracks.cbegin(), tracks.cend(),
    [this](const track &a, const track &b) {
      return lorentz::delta_r(a.p, p) < lorentz::delta_r(b.p, p);
    });
  matched = std::distance(tracks.begin(), it);
}
