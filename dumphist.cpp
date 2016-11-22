
#include <cstdlib>
#include <iostream>

#include "histogram.h"
#include "histogram_reader.h"
#include "serializer.h"

void print_histogram(const hist::histogram &h);

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
  double min = std::numeric_limits<double>::lowest();
  double max = std::numeric_limits<double>::max();
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

  // Read the histogram
  histogram_reader reader(std::cin);
  hist::histogram hist = reader.histogram(name, min, max, bins);
  print_histogram(hist);

  return 0;
}

/**
 * Prints one histogram (horizontally)
 */
void print_histogram(const hist::histogram &h)
{
  // Find maximum value
  double maxy = 0, shown = 0;
  auto end = h.end();
  for (auto it = h.begin(); it != end; ++it) {
    if (*it > maxy) {
      maxy = *it;
      shown += *it;
    }
  }

  // Find the total number of events
  double total = std::accumulate(h.begin(), h.end(), 0);
  total += h.out_of_range().underflow();
  total += h.out_of_range().overflow();

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
