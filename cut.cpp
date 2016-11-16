#include "cut.h"

lua_cut::lua_cut(const std::string &name, const std::string &code) :
  _code(code), _name(name)
{}

sol::load_result lua_cut::load_into(sol::state &lua)
{
  return lua.load(_code);
}
