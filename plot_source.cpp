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
{}

QCPAbstractPlottable *file_plot_source::plot(QCPAxis *x, QCPAxis *y,
                                             const config &config)
{
  histogram_reader reader(_file_path);
  hist::histogram hist = reader.histogram(_name, config.min, config.max,
                                          config.bins);

  QCPGraph *graph = new QCPGraph(x, y);
  hist::qt::set_graph_data(graph, hist);
  graph->setLineStyle(QCPGraph::lsStepCenter);
  return graph;
}
