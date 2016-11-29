#ifndef MULTIPLOT_WINDOW_H
#define MULTIPLOT_WINDOW_H

#include <QColor>
#include <QMainWindow>
#include <QMap>

#include "plot_source.h"

class QCustomPlot;
class QSignalMapper;
class QTreeWidget;
class QTreeWidgetItem;
class QVBoxLayout;

class QCPAbstractPlottable;

class multiplot_window : public QMainWindow
{
  Q_OBJECT

  struct plot_data
  {
    QString path;
    std::string name;
    plot_source *source;
    QTreeWidgetItem *item;
    QCPAbstractPlottable *plottable;
    QColor color;
  };

  QTreeWidget *_tree;
  QCustomPlot *_plot;
  QVBoxLayout *_vbox;
  QMap<QString, plot_data> _data;

  double _min, _max;
  int _bins;
  plot_source::config _config;

public:
  explicit multiplot_window();

private:
  bool is_plot_enabled(const QString &name);
  void enable_plot(const QString &name);
  void disable_plot(const QString &name);
  void update_plots();

  void update_config();
  QColor next_color();

  QWidget *create_config_bar();
  void populate_tree();

private slots:
  void item_double_clicked(QTreeWidgetItem *item);
  void update_min(double min);
  void update_max(double max);
  void update_bins(int bins);
  void set_log_scale(bool log);
};

#endif // MULTIPLOT_WINDOW_H
