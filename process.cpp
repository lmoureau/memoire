
#include <iostream>

#include "serializer.h"

/*
 * Reads events from standard input, runs the program specified on the command
 * line and prints them to standard output.
 */
int main(int argc, char **argv)
{
  // Read the program file name from the command line.
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " program.lua" << std::endl;
    return 1;
  }
  std::string filename = argv[1];

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

  // Load the program
  sol::function program = lua.load_file(filename);

  // Read one event at a time and print them all
  unserializer uns(std::cin);
  serializer ser(std::cout);
  auto lua_e = lua["e"];
  bool eof = false;
  while (std::cin && std::cout) {
    sol::table e = lua.create_table();
    uns.read(lua, e, eof);
    if (eof) {
      break;
    }
    lua_e = e;
    sol::object result = program();
    if (result.get_type() != sol::type::boolean || result.as<bool>()) {
      ser.write(lua_e);
    }
  }

  return 0;
}
