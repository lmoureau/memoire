#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

#include <qcustomplot.h>

#include "parser.h"
#include "run_config.h"

class main_window : public QMainWindow
{
  Q_OBJECT

  run _basic_run;
  run_config *_config;
  parser *_parser;
  QCustomPlot *_plot;
  QListWidget *_plots;
  bool _log_scale;

public:
  explicit main_window(run basic_run, run_config *rc, parser *in);

private slots:
  void refresh_results();
  void save();
  void set_log_scale(bool log);
};

#endif // MAIN_WINDOW_H
