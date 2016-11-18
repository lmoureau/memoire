
#include <iostream>

#include "serializer.h"

void print_table(const sol::table &t, const std::string &indent = "");
void print_value(const sol::object &v, const std::string &indent = "");

/*
 * Reads an event stream from standard input, and prints it to standard output
 * in a human-readable form. The output resembles Lua code, but don't rely on it
 * being correct.
 */
int main(int argc, char **argv)
{
  // Setup lua
  sol::state lua;
  // We'll maybe need these libraries
  lua.open_libraries(sol::lib::base,
                     sol::lib::math,
                     sol::lib::package,
                     sol::lib::table);
  // Change the lua path to include ./lua and ../lua
  std::string oldpath = lua["package"]["path"];
  lua["package"]["path"] = oldpath + ";./lua/?.lua;../lua/?.lua";

  // Read one event at a time and print them all
  unserializer uns(std::cin);
  bool end_of_file = false;
  while (std::cin && !end_of_file) {
    sol::table e;
    uns.read(lua, e, end_of_file);
    if (!end_of_file) {
      std::cout << "=== Next event ===\n";
      print_table(e);
      std::cout << "\n";
    }
  }

  return 0;
}

/*
 * Prints the content of a Lua table to standard output.
 */
void print_table(const sol::table &t, const std::string &indent)
{
  // Print type information if it's in the metatable
  if (t[sol::metatable_key] && t[sol::metatable_key]["__class"] &&
      t[sol::metatable_key]["__module"]) {
    std::string class_name = t[sol::metatable_key]["__class"];
    std::string module_name = t[sol::metatable_key]["__module"];
    std::cout << module_name << "." << class_name << " ";
  }
  std::cout << "{\n";

  t.for_each([indent](const sol::object &key, const sol::object &value) {
    std::cout << indent << "  ";
    // Print the key
    switch (key.get_type()) {
    case sol::type::boolean:
      std::cout << "[" << key.as<bool>() << "]";
      break;
    case sol::type::number:
      std::cout << "[" << key.as<double>() << "]";
      break;
    case sol::type::string:
      std::cout << key.as<std::string>();
      break;
    default:
      std::cout << "<unsupported>";
      break;
    }
    // Print the value
    std::cout << " = ";
    print_value(value, indent + "  ");
    std::cout << ",\n";
  });
  std::cout << indent << "}";
}

/*
 * Prints a lua value to standard output.
 */
void print_value(const sol::object &v, const std::string &indent)
{
  switch (v.get_type()) {
  case sol::type::boolean:
    std::cout << v.as<bool>();
    break;
  case sol::type::number:
    std::cout << v.as<double>();
    break;
  case sol::type::string:
    // There's no problem not escaping the string, because the goal isn't to
    // produce valid Lua anyway.
    std::cout << "\"" << v.as<std::string>() << "\"";
    break;
  case sol::type::table:
    print_table(v.as<sol::table>(), indent);
    break;
  default:
    // Other Lua data types are unsupported.
    std::cout << "<unsupported>";
    break;
  }
}
