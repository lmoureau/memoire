#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <iostream>
#include <unordered_map>
#include <memory>

#include "sol.hpp"

namespace detail
{
  enum class opcode : unsigned char
  {
    end          =  0,
    new_name     =  1,
    new_type     =  2,
    metatable    =  3,
    named_false  = 10,
    named_true   = 11,
    named_number = 12,
    named_string = 13,
    named_table  = 14,
    array_false  = 20,
    array_true   = 21,
    array_number = 22,
    array_string = 23,
    array_table  = 24,
  };
} // namespace detail

class serializer
{
  std::ostream &_out;
  std::unordered_map<std::string, int> _names;
  std::unordered_map<std::string, int> _types;
public:
  explicit serializer(std::ostream &out) : _out(out) {}

  void write(const sol::table &event);

private:
  int name_id(const std::string &name);
  void print_string(const std::string &str);
  void print_table_contents(const sol::table &t);
  void print_value(double id, const sol::object &v);
  void print_value(const std::string &name, const sol::object &v);
};

class unserializer
{
  std::istream &_in;
  std::unordered_map<int, std::string> _names;
  std::unordered_map<int, sol::table> _types;
public:
  explicit unserializer(std::istream &in) : _in(in) {}

  void read(sol::state &lua, sol::table &event, bool &eof);

private:
  double read_id();
  std::string read_name_id();
  std::string read_string();
  sol::table &read_type_id();
  void read_new_name();
  void read_new_type(sol::state &lua);
  void read_table_contents(sol::state &lua, sol::table &t, bool *eof = nullptr);
};

#endif // SERIALIZER_H
