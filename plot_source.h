#ifndef PLOT_SOURCE_H
#define PLOT_SOURCE_H

#include <limits>
#include <string>

#include "qcustomplot.h"

class plot_source : public QObject
{
  Q_OBJECT
public:
  explicit plot_source(QObject *parent = nullptr) : QObject(parent) {}

  virtual QCPAbstractPlottable *plot(QCPAxis *x, QCPAxis *y) = 0;
};

class file_plot_source : public plot_source
{
  Q_OBJECT

  std::string _file_path;
  std::string _name;
  const double _min = std::numeric_limits<double>::lowest();
  const double _max = std::numeric_limits<double>::max();
  const int _bins = 50;

public:
  explicit file_plot_source(const QString &file_path, const std::string &name,
                            QObject *parent = nullptr);

  QCPAbstractPlottable *plot(QCPAxis *x, QCPAxis *y) override;
};

#endif // PLOT_SOURCE_H
