#ifndef LUA_PLOT_SOURCE_H
#define LUA_PLOT_SOURCE_H

#include "plot_source.h"

class lua_plot_source : public plot_source
{
  Q_OBJECT

  QString _expression;

public:
  explicit lua_plot_source(QObject *parent = nullptr);

  void minmax(double &min, double &max) const override;
  QCPAbstractPlottable *plot(QCPAxis *x, QCPAxis *y,
                             const config &config) override;

public slots:
  void set_expression(const QString &expression);
};

#endif // LUA_PLOT_SOURCE_H
