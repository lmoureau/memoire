#include "plot_source.h"

#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>

#include "histogram-qt.h"
#include "histogram_reader.h"

file_plot_source::file_plot_source(const QString &file_path,
                                   const std::string &name,
                                   QObject *parent) :
  plot_source(parent),
  _file_path(file_path.toAscii().constData()),
  _name(name)
{
  histogram_reader reader(_file_path);
  _data = reader.data(name);
}

void file_plot_source::minmax(double &min, double &max) const
{
  double absmin = min;
  double absmax = max;
  min = std::numeric_limits<double>::max();
  max = std::numeric_limits<double>::lowest();
  for (auto &point : _data) {
    if (point.first >= absmin && point.first < min) {
      min = point.first;
    }
    if (point.first < absmax && point.first > max) {
      max = point.first;
    }
  }
}

QCPAbstractPlottable *file_plot_source::plot(QCPAxis *x, QCPAxis *y,
                                             const config &config)
{
  hist::histogram hist = hist::histogram(config.axis);
  for (auto &point : _data) {
    hist.bin(point.first, point.second);
  }

  QCPGraph *graph = new QCPGraph(x, y);
  hist::qt::set_graph_data(graph, hist);
  graph->setLineStyle(QCPGraph::lsStepCenter);
  return graph;
}

double file_plot_source::value_at(double x, const config &config)
{
  hist::histogram hist = hist::histogram(config.axis);
  for (auto &point : _data) {
    hist.bin(point.first, point.second);
  }

  int bin = config.axis(x);
  return hist.data().at(bin);
}
