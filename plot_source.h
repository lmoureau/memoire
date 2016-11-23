#ifndef PLOT_SOURCE_H
#define PLOT_SOURCE_H

#include <limits>
#include <string>

#include "qcustomplot.h"

class plot_source : public QObject
{
  Q_OBJECT
public:
  struct config
  {
    double min, max;
    int bins;
  };

  explicit plot_source(QObject *parent = nullptr) : QObject(parent) {}

  virtual QCPAbstractPlottable *plot(QCPAxis *x, QCPAxis *y,
                                     const config &config) = 0;
};

class file_plot_source : public plot_source
{
  Q_OBJECT

  std::string _file_path;
  std::string _name;

public:
  explicit file_plot_source(const QString &file_path, const std::string &name,
                            QObject *parent = nullptr);

  QCPAbstractPlottable *plot(QCPAxis *x, QCPAxis *y,
                             const config &config) override;
};

#endif // PLOT_SOURCE_H
