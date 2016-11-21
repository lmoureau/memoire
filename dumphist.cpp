
#include <cstdlib>
#include <iostream>

#include "histogram.h"
#include "serializer.h"

void print_histogram(const sol::table &h, double umin, double umax, int bins);

/*
 * Reads an histogram file from standard input and displays one of them in the
 * terminal.
 */
int main(int argc, char **argv)
{
  // Read the program file name from the command line.
  if (argc < 2 || argc > 5) {
    std::cout << "Usage: " << argv[0]
              << " name [min [max [bins]]]" << std::endl;
    return 1;
  }

  // Read arguments
  std::string name = argv[1];
  double min = -1e10, max = 1e10;
  int bins = 20;
  if (argc > 2) {
    min = std::strtod(argv[2], nullptr);
    if (argc > 3) {
      max = std::strtod(argv[3], nullptr);
      if (argc > 4) {
        bins = std::strtol(argv[4], nullptr, 10);
      }
    }
  }

  // Setup lua
  sol::state lua;
  // We'll maybe need these libraries
  lua.open_libraries(sol::lib::base);

  // Read the one entry
  unserializer uns(std::cin);
  sol::table h = lua.create_table();
  bool eof; // unused
  uns.read(lua, h, eof);

  // Print the histogram
  auto value = h[name];
  if (!value.valid()) {
    std::cout << "ERROR: No histogram \"" << name << "\" in input."
              << std::endl;
    return 3;
  }
  print_histogram(value, min, max, bins);

  return 0;
}

/**
 * Prints one histogram (horizontally)
 */
void print_histogram(const sol::table &t, double umin, double umax, int bins)
{
  // Find the bounds
  double max = -1e10, min = 1e10;
  t.for_each([&](const sol::object &key, const sol::object &value) {
    if (key.get_type() == sol::type::number &&
        value.get_type() == sol::type::number) {
      if (max < key.as<double>() && key.as<double>() < umax) {
        max = key.as<double>();
      }
      if (min > key.as<double>() && key.as<double>() > umin) {
        min = key.as<double>();
      }
    }
  });

  // Accumulate
  double total = 0;
  auto h = hist::histogram(min, max, bins);
  t.for_each([&](const sol::object &key, const sol::object &value) {
    if (key.get_type() == sol::type::number &&
        value.get_type() == sol::type::number) {
      h.bin(key.as<double>(), value.as<double>());
      total += value.as<double>();
    }
  });

  // Find maximum value
  double maxy = 0, shown = 0;
  auto end = h.end();
  for (auto it = h.begin(); it != end; ++it) {
    if (*it > maxy) {
      maxy = *it;
      shown += *it;
    }
  }

  // Print
  std::cout << "Showing " << shown << " out of " << total << ".\n";

  auto it = h.begin();
  for (int i = 0; it != end; ++it, ++i) {
    std::cout.width(5);
    std::cout.precision(3);
    std::cout << std::right << h.axis().bin_center(i) << " |";
    for (int i = 0; i < 100 * *it / maxy; ++i) {
      std::cout << "#";
    }
    std::cout << "\n";
  }
}
