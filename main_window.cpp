#include "main_window.h"

#include <QSplitter>
#include <QToolBar>
#include <QToolButton>

#include "histogram-qt.h"

main_window::main_window(run basic_run, run_config *rc, parser *in) :
  _basic_run(basic_run),
  _config(rc),
  _parser(in),
  _plot(new QCustomPlot),
  _log_scale(false)
{
  setWindowTitle("Hé Hé");

  _plot->setAutoAddPlottableToLegend(true);
  _plot->legend->setVisible(true);
  _plot->axisRect()->setRangeZoom(Qt::Horizontal);
  _plot->axisRect()->setRangeDrag(Qt::Horizontal);
  _plot->axisRect()->setupFullAxesBox(true);
  _plot->setInteractions(
      QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
  _plot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  resize(600, 400);

  refresh_results();
  connect(_config, SIGNAL(config_changed()), this, SLOT(refresh_results()));

  QSplitter *splitter = new QSplitter(Qt::Horizontal);
  splitter->addWidget(_config);
  splitter->addWidget(_plot);
  setCentralWidget(splitter);

  QToolBar *tools = new QToolBar();
  addToolBar(tools);
  tools->setFloatable(false);

  QAction *action = tools->addAction("Log scale");
  action->setCheckable(true);
  connect(action, SIGNAL(toggled(bool)), this, SLOT(set_log_scale(bool)));
}

void main_window::refresh_results()
{
  run r = _basic_run;
  _config->fill_run(r);
  _parser->reset();
  run::result result = r(_parser);

  _plot->clearGraphs();

  _plot->addGraph();
  hist::qt::set_graph_data(_plot->graph(), result.before_cuts.at("mass"));
  _plot->graph()->setName("Before cuts");
  _plot->graph()->setLineStyle(QCPGraph::lsStepCenter);

  _plot->addGraph();
  hist::qt::set_graph_data(_plot->graph(), result.after_cuts.at("mass"));
  _plot->graph()->setName("After cuts");
  _plot->graph()->setBrush(QBrush(Qt::blue, Qt::Dense6Pattern));
  _plot->graph()->setLineStyle(QCPGraph::lsStepCenter);

  if (_log_scale) {
    _plot->yAxis->setScaleType(QCPAxis::stLogarithmic);
  } else {
    _plot->yAxis->setScaleType(QCPAxis::stLinear);
  }

  _plot->xAxis->rescale();
  _plot->yAxis->rescale();

  _plot->replot();
}

void main_window::set_log_scale(bool log)
{
  _log_scale = log;
  if (log) {
    _plot->yAxis->setScaleType(QCPAxis::stLogarithmic);
  } else {
    _plot->yAxis->setScaleType(QCPAxis::stLinear);
  }
  _plot->replot();
}
