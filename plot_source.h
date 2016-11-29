#ifndef PLOT_SOURCE_H
#define PLOT_SOURCE_H

#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "qcustomplot.h"

#include "histogram.h"

class plot_source : public QObject
{
  Q_OBJECT
public:
  struct config
  {
    hist::linear_axis<double> axis;
  };

  explicit plot_source(QObject *parent = nullptr) : QObject(parent) {}

  virtual void minmax(double &min, double &max) const = 0;
  virtual QCPAbstractPlottable *plot(QCPAxis *x, QCPAxis *y,
                                     const config &config) = 0;
};

class file_plot_source : public plot_source
{
  Q_OBJECT

  std::string _file_path;
  std::string _name;
  std::vector<std::pair<double, double>> _data;

public:
  explicit file_plot_source(const QString &file_path, const std::string &name,
                            QObject *parent = nullptr);

  void minmax(double &min, double &max) const override;
  QCPAbstractPlottable *plot(QCPAxis *x, QCPAxis *y,
                             const config &config) override;
};

#endif // PLOT_SOURCE_H
