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
    plot_source *source;
    QTreeWidgetItem *item;
    QCPAbstractPlottable *plottable;
    QColor color;
  };

  QTreeWidget *_tree;
  QCustomPlot *_plot;
  QVBoxLayout *_vbox;
  QMap<QString, plot_data> _data;
  int _last_color = 0;

  plot_source::config _config;

public:
  explicit multiplot_window();

private:
  bool is_plot_enabled(const QString &name);
  void enable_plot(const QString &name);
  void disable_plot(const QString &name);
  void update_plots();

  QColor next_color();

  QWidget *create_config_bar();
  void populate_tree();

private slots:
  void item_double_clicked(QTreeWidgetItem *item);
  void update_min(double min);
  void update_max(double max);
  void update_bins(int bins);
};

#endif // MULTIPLOT_WINDOW_H
