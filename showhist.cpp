
#include <iostream>

#include <QApplication>
#include <QDir>

#include "histogram-qt.h"
#include "histogram_reader.h"

int main(int argc, char **argv)
{
  // QApplication will remove the arguments it understands
  QApplication app(argc, argv);

  // Read the program file name from the command line.
  if (argc < 2 || argc > 5) {
    std::cout << "Usage: " << argv[0]
              << " name [min [max [bins]]]" << std::endl;
    return 1;
  }

  // Read arguments
  std::string name = argv[1];
  double min = std::numeric_limits<double>::lowest();
  double max = std::numeric_limits<double>::max();
  int bins = 50;
  if (argc > 2) {
    min = QString(argv[2]).toDouble();
    if (argc > 3) {
      max = QString(argv[3]).toDouble();
      if (argc > 4) {
        bins = QString(argv[4]).toInt();
      }
    }
  }

  // Read the histogram
  histogram_reader reader(std::cin);
  hist::histogram hist = reader.histogram(name, min, max, bins);

  // Create widget
  QCustomPlot *widget = new QCustomPlot();
  widget->setWindowTitle(name.c_str());
  widget->axisRect()->setRangeZoom(Qt::Horizontal);
  widget->axisRect()->setRangeDrag(Qt::Horizontal);
  widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
  widget->resize(600, 400);

  // Add the graph
  widget->addGraph();
  hist::qt::set_graph_data(widget->graph(), hist);
  widget->graph()->setLineStyle(QCPGraph::lsStepCenter);

  widget->xAxis->rescale();
  widget->yAxis->rescale();
  widget->replot();

  // Show!
  widget->show();
  return app.exec();
}
