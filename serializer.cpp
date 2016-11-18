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
  // Print type information if it's in the metatable
  if (t[sol::metatable_key] && t[sol::metatable_key]["__class"] &&
      t[sol::metatable_key]["__module"]) {
    std::string class_name = t[sol::metatable_key]["__class"];
    std::string module_name = t[sol::metatable_key]["__module"];
    if (_types.count(class_name + "@" + module_name) == 0) {
      // Print type infomation
      _out << "2 ";
      print_string(class_name);
      print_string(module_name);
      int id = _types.size();
      _out << id << " ";
      // Add the type to the table
      _types[class_name + "@" + module_name] = id;
    }
    int id = _types.at(class_name + "@" + module_name);
    std::cout << "3 " << id << " ";
  }
  // Print values
  t.for_each([this](const sol::object &key, const sol::object &value) {
    sol::type type = key.get_type();
    // Only numbers and strings are allowed as indices for serialization
    assert(type == sol::type::number || type == sol::type::string);

    if (type == sol::type::string) {
      print_value(key.as<std::string>(), value);
    } else {
      // sol::type::number
      print_value(key.as<double>(), value);
    }
  });
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

void unserializer::read(sol::state &lua, sol::table &event, bool &eof)
{
  event = lua.create_table();
  read_table_contents(lua, event, &eof);
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

sol::table &unserializer::read_type_id()
{
  int type_id;
  _in >> type_id;
  assert(_types.count(type_id) != 0);
  return _types.at(type_id);
}

void unserializer::read_new_name()
{
  std::string name = read_string();
  int id;
  _in >> id;
  assert(_names.count(id) == 0);
  _names.insert(std::make_pair(id, name));
}

void unserializer::read_new_type(sol::state &lua)
{
  std::string type_name = read_string();
  std::string module_name = read_string();
  int id;
  _in >> id;
  assert(_types.count(id) == 0);
  try {
    lua["require"](module_name);
  } catch(sol::error e) {
    std::cerr << "[WARN] Module \"" << module_name << "\" could not be loaded."
              << " Data is safe, but some class members will not be available."
              << std::endl;
    _types[id] = lua.create_table();
    return;
  }
  try {
    sol::table metatable = lua[type_name];
    _types[id] = metatable;
    if (metatable["__class"].get<std::string>() != type_name) {
      std::cerr << "[WARN] Class \"" << type_name << "\" could be loaded,"
                << " but was renamed to \""
                << metatable["__class"].get<std::string>() << "\""
                << std::endl;
    }
    if (metatable["__module"].get<std::string>() != module_name) {
      std::cerr << "[WARN] Module \"" << module_name << "\" could be loaded,"
                << " but was renamed to \""
                << metatable["__module"].get<std::string>() << "\""
                << std::endl;
    }
  } catch(sol::error e) {
    std::cerr << "[WARN] Class \"" << type_name << "\" could not be loaded."
              << " Data is safe, but class members will not be available."
              << std::endl;
    _types[id] = lua.create_table();
    return;
  }
}

void unserializer::read_table_contents(sol::state &lua, sol::table &t,
                                       bool *eof)
{
  // An empty event means eof. We set it to true here, and then to false when we
  // encounter a value.
  bool fake_eof;
  bool &ref_eof = (eof != nullptr ? *eof : fake_eof);
  ref_eof = true;

  // As the metatable can prevent adding new data, we need to set it at the very
  // end. As we can read it any time, we need to save it temporarily.
  bool has_metatable = false;
  sol::table metatable;

  using detail::opcode;
  while (true) {
    opcode code;
    _in >> (int&) code;

    std::string name;
    double id, value;
    sol::table tab;

    switch (code) {
    case opcode::end:
      // Set the metatable if there was one.
      if (has_metatable) {
        t[sol::metatable_key] = metatable;
      }
      return;
    case opcode::new_name:
      read_new_name();
      break;
    case opcode::new_type:
      read_new_type(lua);
      break;
    case opcode::metatable:
      metatable = read_type_id();
      has_metatable = true;
      break;
    case opcode::named_false:
      ref_eof = false;
      t[read_name_id()] = false;
      break;
    case opcode::named_true:
      ref_eof = false;
      t[read_name_id()] = true;
      break;
    case opcode::named_number:
      ref_eof = false;
      name = read_name_id();
      _in >> value;
      t[name] = value;
      break;
    case opcode::named_string:
      ref_eof = false;
      name = read_name_id();
      t[name] = read_string();
      break;
    case opcode::named_table:
      ref_eof = false;
      name = read_name_id();
      tab = lua.create_table();
      read_table_contents(lua, tab);
      t[name] = tab;
      break;
    case opcode::array_false:
      ref_eof = false;
      t[read_id()] = false;
      break;
    case opcode::array_true:
      ref_eof = false;
      t[read_id()] = true;
      break;
    case opcode::array_number:
      ref_eof = false;
      id = read_id();
      _in >> value;
      t[id] = value;
      break;
    case opcode::array_string:
      ref_eof = false;
      id = read_id();
      t[id] = read_string();
      break;
    case opcode::array_table:
      ref_eof = false;
      id = read_id();
      tab = lua.create_table();
      read_table_contents(lua, tab);
      t[id] = tab;
      break;
    }
  }
}
