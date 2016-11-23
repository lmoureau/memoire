#include "multiplot_window.h"

#include <cassert>

#include <QDir>
#include <QIcon>
#include <QSplitter>
#include <QSignalMapper>
#include <QTreeWidget>
#include <QVBoxLayout>

#include "histogram_reader.h"
#include "plot_source.h"
#include "qcustomplot.h"

multiplot_window::multiplot_window() :
  _tree(new QTreeWidget),
  _plot(new QCustomPlot),
  _vbox(new QVBoxLayout)
{
  QSplitter *splitter = new QSplitter(Qt::Horizontal);
  setCentralWidget(splitter);

  splitter->addWidget(_tree);
  _tree->setHeaderLabel(tr("Name"));
  populate_tree();
  connect(_tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
          this, SLOT(item_double_clicked(QTreeWidgetItem *)));

  _vbox = new QVBoxLayout;
  QWidget *vboxw = new QWidget;
  vboxw->setLayout(_vbox);
  splitter->addWidget(vboxw);

  _vbox->addWidget(_plot);
  _vbox->setStretchFactor(_plot, 100);
  _plot->axisRect()->setRangeZoom(Qt::Horizontal);
  _plot->axisRect()->setRangeDrag(Qt::Horizontal);
  _plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}

bool multiplot_window::is_plot_enabled(const QString &name)
{
  assert(_data.contains(name));
  return _data[name].plottable != nullptr;
}

void multiplot_window::enable_plot(const QString &name)
{
  assert(_data.contains(name));
  auto &data = _data[name];

  data.item->setIcon(0, QIcon::fromTheme("status-ok"));

  assert(data.plottable == nullptr);
  data.plottable = data.source->plot(_plot->xAxis, _plot->yAxis);
  _plot->addPlottable(data.plottable);
  _plot->xAxis->rescale();
  _plot->yAxis->rescale();
  _plot->replot();
}

void multiplot_window::disable_plot(const QString &name)
{
  assert(_data.contains(name));
  auto &data = _data[name];

  data.item->setIcon(0, QIcon());

  assert(data.plottable != nullptr);
  _plot->removePlottable(data.plottable);
  // removePlottable deletes the plottable for us
  data.plottable = nullptr;
  _plot->xAxis->rescale();
  _plot->yAxis->rescale();
  _plot->replot();
}

void multiplot_window::item_double_clicked(QTreeWidgetItem *item)
{
  QString name = qvariant_cast<QString>(item->data(0, Qt::UserRole));
  if (is_plot_enabled(name)) {
    disable_plot(name);
  } else {
    enable_plot(name);
  }
}

void multiplot_window::populate_tree()
{
  QDir dir("data");
  for (auto info: dir.entryInfoList(QDir::Files, QDir::Name)) {
    if (info.fileName().endsWith(".hist")) {
      QTreeWidgetItem *item = new QTreeWidgetItem();
      _tree->addTopLevelItem(item);
      item->setText(0, info.fileName());
      item->setExpanded(true);

      histogram_reader reader(info.filePath().toStdString());
      auto names = reader.names();
      for (auto hname: names) {
        file_plot_source *src = new file_plot_source(info.filePath(), hname);
        QString name = info.fileName() + "#" + hname.c_str();

        QTreeWidgetItem *child = new QTreeWidgetItem(item);
        child->setText(0, hname.c_str());
        child->setData(0, Qt::UserRole, name);

        _data[name] = plot_data{ src, child, nullptr };
      }
    }
  }
}
