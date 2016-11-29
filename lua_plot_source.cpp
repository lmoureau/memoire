#include "lua_plot_source.h"

#include "sol.hpp"

lua_plot_source::lua_plot_source(QObject *parent) :
  plot_source(parent)
{}

void lua_plot_source::add_global(const QString &name, plot_source *source)
{
  if (source != this) {
    _globals.push_back(global{ name, source });
  }
}

void lua_plot_source::minmax(double &min, double &max) const
{}

QCPAbstractPlottable *lua_plot_source::plot(QCPAxis *x, QCPAxis *y,
                                            const config &config)
{
  QCPGraph *graph = new QCPGraph(x, y);

  // Init lua
  sol::state lua;
  lua.open_libraries(sol::lib::base, sol::lib::math);

  // Copy the math library as globals
  sol::table math = lua["math"];
  math.for_each([&lua](const sol::object &key, const sol::object &value) {
    lua[key] = value;
  });

  try {
    // Parse the expression
    sol::load_result lr = lua.load(_expression.toLocal8Bit().constData());
    if (!lr.valid()) {
      throw sol::error(lr.get<std::string>());
    }
    sol::protected_function function = lr;

    // Evaluate
    auto x = lua["x"];
    for (int i = 0; i < config.axis.bin_count(); ++i) {
      x = config.axis.bin_center(i);

      // Add global variables coming from other sources
      for (auto &g : _globals) {
        lua[g.name.toLocal8Bit().constData()] = g.source->value_at(x, config);
      }

      auto result = function();
      if (result.valid()) {
        sol::object val = result.get<sol::object>();
        if (val.get_type() == sol::type::number) {
          graph->addData(x, val.as<double>());
        } else {
          throw sol::error("Expression didn't return a number");
        }
      } else {
        throw sol::error("Invalid expression");
      }
    }
  } catch(sol::error &e) {
    std::cerr << "WARNING: " << e.what() << std::endl;
    graph->deleteLater();
    return nullptr;
  }

  return graph;
}

void lua_plot_source::set_expression(const QString &expression)
{
  _expression = "return " + expression;
}
