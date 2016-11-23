
#include <iostream>

#include "serializer.h"

/*
 * Reads events from standard input, runs the program specified on the command
 * line and prints them to standard output.
 */
int main(int argc, char **argv)
{
  // Read the program file name from the command line.
  if (argc != 2 && argc != 3) {
    std::cout << "Usage: " << argv[0] << " [not] program.lua" << std::endl;
    return 1;
  }
  bool negate = false;
  if (argc == 3 && std::string(argv[1]) == "not") {
    negate = true;
  } else if (argc == 3) {
    // Argument 1 isn't "not"
    std::cout << "Usage: " << argv[0] << " [not] program.lua" << std::endl;
    return 1;
  }
  std::string filename = argv[argc - 1];

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
    auto result = program();
    if (result.valid()) {
      sol::object val = result.get<sol::object>();
      bool passed = (val.get_type() != sol::type::boolean || val.as<bool>());
      if (passed == !negate) {
        ser.write(lua_e);
      }
    } else {
      std::cerr << "ERROR: " << result.get<sol::error>().what() << std::endl;
      return 3;
    }
  }

  return 0;
}
