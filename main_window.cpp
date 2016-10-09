#include "main_window.h"

#include <QAction>
#include <QSplitter>
#include <QToolBar>
#include <QToolButton>

#include "histogram-qt.h"

main_window::main_window(run basic_run, run_config *rc, parser *in) :
  _basic_run(basic_run),
  _config(rc),
  _parser(in),
  _plot(new QCustomPlot),
  _plots(new QListWidget),
  _log_scale(false)
{
  setWindowTitle("He He");

  _plot->setAutoAddPlottableToLegend(true);
  _plot->legend->setVisible(true);
  _plot->axisRect()->setRangeZoom(Qt::Horizontal);
  _plot->axisRect()->setRangeDrag(Qt::Horizontal);
  _plot->setInteractions(
      QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
  _plot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  _plot->yAxis2->setTicks(true);
  _plot->yAxis2->setTickLabels(true);
  _plot->yAxis2->setVisible(true);
  resize(600, 400);

  refresh_results();
  connect(_config, SIGNAL(config_changed()), this, SLOT(refresh_results()));
  connect(_plots, SIGNAL(currentRowChanged(int)),
          this, SLOT(refresh_results()));

  QSplitter *splitter = new QSplitter(Qt::Horizontal);
  splitter->addWidget(_config);
  splitter->addWidget(_plots);
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

  QStringList plot_names;
  for (const auto &element : result.histos) {
    plot_names << element.first.c_str();
  }
  int selected = _plots->currentRow();
  selected = selected < 0 ? 0 : selected;
  _plots->clear();
  _plots->addItems(plot_names);
  _plots->blockSignals(true);
  _plots->setCurrentRow(selected);
  _plots->blockSignals(false);

  std::string name = plot_names[selected].toLatin1().data();

  _plot->clearPlottables();

  hist::qt::histogram2d_plottable *migration =
      new hist::qt::histogram2d_plottable(
        _plot->xAxis,
        _plot->yAxis2,
        result.histos.at(name).migration);
  migration->setName("Migration matrix");
  migration->setBrush(QBrush(Qt::darkGreen));
  _plot->addPlottable(migration);

  _plot->addGraph();
  hist::qt::set_graph_data(_plot->graph(),
                           result.histos.at(name).before_cuts);
  _plot->graph()->setName("Before cuts");
  _plot->graph()->setLineStyle(QCPGraph::lsStepCenter);

  _plot->addGraph();
  hist::qt::set_graph_data(_plot->graph(),
                           result.histos.at(name).after_cuts);
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
  _plot->yAxis2->rescale();

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
