
#include <iostream>

#include "parsers.h"
#include "serializer.h"

/*
 * Reads events from a ROOT file and prints them in serialized form to standard
 * output.
 */
int main(int argc, char **argv)
{
  // Read the file name from the command line.
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " file.root" << std::endl;
    return 1;
  }
  std::string filename = argv[1];
  hlt_parser in(filename);

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
  // The parser might want to load additionnal libraries
  in.prepare(lua);

  // Read one event at a time and print them all
  serializer ser(std::cout);
  while (std::cout && !in.end()) {
    sol::table e = lua.create_table();
    in.read();
    if (in.has_rec()) {
      in.fill_rec(lua, e);
      ser.write(e);
    }
  }

  return 0;
}
