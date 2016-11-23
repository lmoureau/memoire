#ifndef MULTIPLOT_WINDOW_H
#define MULTIPLOT_WINDOW_H

#include <QMainWindow>
#include <QMap>

class QCustomPlot;
class QSignalMapper;
class QTreeWidget;
class QTreeWidgetItem;
class QVBoxLayout;

class QCPAbstractPlottable;

class plot_source;

class multiplot_window : public QMainWindow
{
  Q_OBJECT

  struct plot_data
  {
    plot_source *source;
    QTreeWidgetItem *item;
    QCPAbstractPlottable *plottable;
  };

  QTreeWidget *_tree;
  QCustomPlot *_plot;
  QVBoxLayout *_vbox;
  QMap<QString, plot_data> _data;

public:
  explicit multiplot_window();

private:
  bool is_plot_enabled(const QString &name);
  void enable_plot(const QString &name);
  void disable_plot(const QString &name);

  void populate_tree();

private slots:
  void item_double_clicked(QTreeWidgetItem *item);
};

#endif // MULTIPLOT_WINDOW_H
