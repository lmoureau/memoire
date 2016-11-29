#include "histogram_reader.h"

#include <fstream>
#include <stdexcept>

#include "serializer.h"

histogram_reader::histogram_reader(const std::string &filename)
{
  // Open the file
  std::ifstream in(filename);

  // We'll maybe need these libraries
  _lua.open_libraries(sol::lib::base);

  // Read the one entry
  unserializer uns(in);
  _data = _lua.create_table();
  bool eof; // unused
  uns.read(_lua, _data, eof);
}

histogram_reader::histogram_reader(std::istream &in)
{
  // We'll maybe need these libraries
  _lua.open_libraries(sol::lib::base);

  // Read the one entry
  unserializer uns(in);
  _data = _lua.create_table();
  bool eof; // unused
  uns.read(_lua, _data, eof);
}

std::vector<std::string> histogram_reader::names() const
{
  std::vector<std::string> ret;
  _data.for_each([&ret](const sol::object &key, const sol::object &value) {
    if (key.get_type() == sol::type::string &&
        value.get_type() == sol::type::table) {
      ret.push_back(key.as<std::string>());
    }
  });
  return ret;
}

std::vector<std::pair<double, double>>
histogram_reader::data(const std::string &name) const
{
  // Find the histogram data
  sol::object data = _data[name];
  if (data.get_type() != sol::type::table) {
    throw std::range_error("No histogram named \"" + name + "\" found!");
  }
  sol::table t = data;

  // Copy it
  auto vector = std::vector<std::pair<double, double>>();
  t.for_each([&](const sol::object &key, const sol::object &value) {
    if (key.get_type() == sol::type::number &&
        value.get_type() == sol::type::number) {
      vector.push_back(std::make_pair(key.as<double>(), value.as<double>()));
    }
  });

  return vector;
}

hist::histogram histogram_reader::histogram(const std::string &name,
                                            double min, double max,
                                            int nbins) const
{
  // Avoid bad parameters
  if (min >= max) {
    throw std::logic_error("Cannot make histogram with min >= max!");
  }
  if (nbins <= 0) {
    throw std::logic_error("Cannot make histogram with less than 1 bin!");
  }

  // Find the histogram data
  sol::object data = _data[name];
  if (data.get_type() != sol::type::table) {
    throw std::range_error("No histogram named \"" + name + "\" found!");
  }
  sol::table t = data;

  // Find the bounds
  double data_min = std::numeric_limits<double>::max();
  double data_max = std::numeric_limits<double>::lowest();
  t.for_each([&](const sol::object &key, const sol::object &value) {
    if (key.get_type() == sol::type::number &&
        value.get_type() == sol::type::number) {
      if (data_max < key.as<double>() && key.as<double>() < max) {
        data_max = key.as<double>();
      }
      if (data_min > key.as<double>() && key.as<double>() >= min) {
        data_min = key.as<double>();
      }
    }
  });

  // Avoid empty ranges
  if (data_max == data_min) {
    data_min = min;
    data_max = max;
  }

  // Accumulate data
  auto h = hist::histogram(data_min, data_max, nbins);
  t.for_each([&](const sol::object &key, const sol::object &value) {
    if (key.get_type() == sol::type::number &&
        value.get_type() == sol::type::number) {
      h.bin(key.as<double>(), value.as<double>());
    }
  });

  return h;
}
