#include "serializer.h"

#include <cassert>

void serializer::write(const sol::table &event)
{
  print_table_contents(event);
  _out << "0\n";
}

int serializer::name_id(const std::string &name)
{
  if (_names.count(name) == 0) {
    int id = _names.size();
    _out << "1 ";
    print_string(name);
    _out << id << " ";
    _names.insert(std::make_pair(name, id));
    return id;
  } else {
    return _names[name];
  }
}

void serializer::print_string(const std::string &str)
{
  _out << str.size() << " " << str << " ";
}

void serializer::print_table_contents(const sol::table &t)
{
  for (auto &x : t) {
    sol::type type = x.first.get_type();
    // Only numbers and strings are allowed as indices for serialization
    assert(type == sol::type::number || type == sol::type::string);

    if (type == sol::type::string) {
      print_value(x.first.as<std::string>(), x.second);
    } else {
      // sol::type::number
      print_value(x.first.as<double>(), x.second);
    }
  }
}

void serializer::print_value(double id, const sol::object &v)
{
  sol::type type = v.get_type();
  if (type == sol::type::boolean) {
    if (v.as<bool>()) {
      _out << "21 " << id << " ";
    } else {
      _out << "20 " << id << " ";
    }
  } else if (type == sol::type::number) {
    _out << "22 " << id << " " << v.as<double>() << " ";
  } else if (type == sol::type::string) {
    _out << "23 " << id << " ";
    print_string(v.as<std::string>());
  } else if (type == sol::type::table) {
    _out << "24 " << id << " "; // table begin
    print_table_contents(v.as<sol::table>());
    _out << "0 "; // end
  } else {
    throw 0;
  }
}

void serializer::print_value(const std::string &name, const sol::object &v)
{
  int id = name_id(name);
  sol::type type = v.get_type();
  if (type == sol::type::boolean) {
    if (v.as<bool>()) {
      _out << "11 " << id << " ";
    } else {
      _out << "10 " << id << " ";
    }
  } else if (type == sol::type::number) {
    _out << "12 " << id << " " << v.as<double>() << " ";
  } else if (type == sol::type::string) {
    _out << "13 " << id << " ";
    print_string(v.as<std::string>());
  } else if (type == sol::type::table) {
    _out << "14 " << id << " "; // table begin
    print_table_contents(v.as<sol::table>());
    _out << "0 "; // end
  } else {
    throw 0;
  }
}

void unserializer::read(sol::state &lua, sol::table &event)
{
  event = lua.create_table();
  read_table_contents(lua, event);
}

double unserializer::read_id()
{
  double id;
  _in >> id;
  return id;
}

std::string unserializer::read_name_id()
{
  int name_id;
  _in >> name_id;
  assert(_names.count(name_id) != 0);
  return _names.at(name_id);
}

std::string unserializer::read_string()
{
  unsigned length;
  _in >> length;
  std::string data;
  _in >> data; // TODO spaces in strings
  assert(data.size() == length);
  return data;
}

void unserializer::read_new_name()
{
  std::string name = read_string();
  int id;
  _in >> id;
  assert(_names.count(id) == 0);
  _names.insert(std::make_pair(id, name));
}

void unserializer::read_table_contents(sol::state &lua, sol::table &t)
{
  using detail::opcode;
  while (true) {
    opcode code;
    _in >> (int&) code;

    std::string name;
    double id, value;
    sol::table tab;

    switch (code) {
    case opcode::end:
      return;
    case opcode::new_name:
      read_new_name();
      break;
    case opcode::named_false:
      t[read_name_id()] = false;
      break;
    case opcode::named_true:
      t[read_name_id()] = true;
      break;
    case opcode::named_number:
      name = read_name_id();
      _in >> value;
      t[name] = value;
      break;
    case opcode::named_string:
      name = read_name_id();
      t[name] = read_string();
      break;
    case opcode::named_table:
      name = read_name_id();
      tab = lua.create_table();
      read_table_contents(lua, tab);
      t[name] = tab;
      break;
    case opcode::array_false:
      t[read_id()] = false;
      break;
    case opcode::array_true:
      t[read_id()] = true;
      break;
    case opcode::array_number:
      id = read_id();
      _in >> value;
      t[id] = value;
      break;
    case opcode::array_string:
      id = read_id();
      t[id] = read_string();
      break;
    case opcode::array_table:
      id = read_id();
      tab = lua.create_table();
      read_table_contents(lua, tab);
      t[id] = tab;
      break;
    }
  }
}
