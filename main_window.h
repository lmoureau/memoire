#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

#include <qcustomplot.h>

#include "event_source.h"
#include "run_config.h"

class main_window : public QMainWindow
{
  Q_OBJECT

  run _basic_run;
  run::result _result;
  run_config *_config;
  event_source *_event_source;
  QCustomPlot *_plot;
  QListWidget *_plots;
  bool _log_scale;

public:
  explicit main_window(run basic_run, run_config *rc, event_source *in);

private slots:
  std::string current_plot_name() const;

  void refresh_results();
  void show_plot(int index);
  void save();
  void save_data();
  void set_log_scale(bool log);
};

#endif // MAIN_WINDOW_H
