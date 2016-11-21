
#include <iostream>

#include "serializer.h"

/*
 * Reads events from standard input, runs the program specified on the command
 * line and prints an histogram list to standard output.
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
  sol::load_result lr = lua.load_file(filename);
  if (!lr.valid()) {
    std::cerr << "ERROR: Could not load script: "
              << lr.get<std::string>() << std::endl;
    return 2;
  }
  sol::protected_function program = lr;

  // Load the histogram library and create the H variable
  lua.script("require \"histogram\"; H = histogram_list.new()");

  // Read one event at a time and print them all
  unserializer uns(std::cin);
  auto lua_e = lua["e"];
  bool eof = false;
  while (std::cin && std::cout) {
    sol::table e = lua.create_table();
    uns.read(lua, e, eof);
    if (eof) {
      break;
    }
    lua_e = e;
    auto result = program();
    if (!result.valid()) {
      std::cerr << "ERROR: " << result.get<sol::error>().what() << std::endl;
      return 3;
    }
  }

  // Write the histograms to stdout
  serializer ser(std::cout);
  ser.write(lua["H"]);

  return 0;
}
