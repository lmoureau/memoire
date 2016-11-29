#include "multiplot_window.h"

#include <cassert>

#include <QCheckBox>
#include <QDir>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QSpinBox>
#include <QSplitter>
#include <QSignalMapper>
#include <QTreeWidget>
#include <QVBoxLayout>

#include "histogram_reader.h"
#include "qcustomplot.h"

multiplot_window::multiplot_window() :
  _tree(new QTreeWidget),
  _plot(new QCustomPlot),
  _vbox(new QVBoxLayout),
  _min(0), _max(2), _bins(100),
  _config(plot_source::config{ hist::linear_axis<double>(_min, _max, _bins) })
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

  _vbox->addWidget(create_config_bar());

  _vbox->addWidget(_plot);
  _vbox->setStretchFactor(_plot, 100);
  _plot->axisRect()->setRangeZoom(Qt::Horizontal);
  _plot->axisRect()->setRangeDrag(Qt::Horizontal);
  _plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
  _plot->setAutoAddPlottableToLegend(true);
  _plot->legend->setVisible(true);
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
  data.plottable = data.source->plot(_plot->xAxis, _plot->yAxis, _config);
  data.plottable->setName(name);
  data.color = next_color();
  data.plottable->setPen(data.color);
  data.item->setData(0, Qt::TextColorRole, data.color);
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
  data.item->setData(0, Qt::TextColorRole, QVariant());
  _plot->xAxis->rescale();
  _plot->yAxis->rescale();
  _plot->replot();
}

void multiplot_window::update_plots()
{
  for (auto &data: _data) {
    if (data.plottable != nullptr) {
      QString name = data.plottable->name();
      _plot->removePlottable(data.plottable);
      data.plottable = data.source->plot(_plot->xAxis, _plot->yAxis, _config);
      data.plottable->setName(name);
      data.plottable->setPen(data.color);
      _plot->addPlottable(data.plottable);
      _plot->xAxis->rescale();
      _plot->yAxis->rescale();
      _plot->replot();
    }
  }
}

QColor multiplot_window::next_color()
{
  static const QColor colors[] = {
    Qt::blue, Qt::red, Qt::darkGreen, Qt::magenta, Qt::black
  };
  for (auto &color: colors) {
    bool used = false;
    for (auto &data: _data) {
      if (data.plottable != nullptr && data.color == color) {
        used = true;
        break;
      }
    }
    if (!used) {
      return color;
    }
  }
  return colors[0];
}

void multiplot_window::item_double_clicked(QTreeWidgetItem *item)
{
  QVariant variant = item->data(0, Qt::UserRole);
  if (variant.isValid()) {
    QString name = qvariant_cast<QString>(variant);
    if (is_plot_enabled(name)) {
      disable_plot(name);
    } else {
      enable_plot(name);
    }
  }
}

QWidget *multiplot_window::create_config_bar()
{
  QWidget *widget = new QWidget;
  QHBoxLayout *layout = new QHBoxLayout;
  layout->setContentsMargins(0, 0, 0, 0);
  widget->setLayout(layout);

  QLabel *label = new QLabel(tr("M&in:"));
  layout->addWidget(label);

  QDoubleSpinBox *box = new QDoubleSpinBox;
  box->setMinimum(std::numeric_limits<double>::lowest());
  box->setMaximum(std::numeric_limits<double>::max());
  box->setValue(_min);
  label->setBuddy(box);
  connect(box, SIGNAL(valueChanged(double)), this, SLOT(update_min(double)));
  layout->addWidget(box);

  label = new QLabel(tr("M&ax:"));
  layout->addWidget(label);

  box = new QDoubleSpinBox;
  box->setMinimum(std::numeric_limits<double>::lowest());
  box->setMaximum(std::numeric_limits<double>::max());
  box->setValue(_max);
  label->setBuddy(box);
  connect(box, SIGNAL(valueChanged(double)), this, SLOT(update_max(double)));
  layout->addWidget(box);

  label = new QLabel(tr("&Bins:"));
  layout->addWidget(label);

  QSpinBox *ibox = new QSpinBox;
  ibox->setMinimum(1);
  ibox->setMaximum(1000);
  ibox->setValue(_bins);
  label->setBuddy(ibox);
  connect(ibox, SIGNAL(valueChanged(int)), this, SLOT(update_bins(int)));
  layout->addWidget(ibox);

  layout->addStretch(100);

  QCheckBox *check = new QCheckBox(tr("&Log scale"));
  connect(check, SIGNAL(toggled(bool)), this, SLOT(set_log_scale(bool)));
  layout->addWidget(check);

  return widget;
}

void multiplot_window::populate_tree()
{
  QDir dir("analysis");
  for (auto info: dir.entryInfoList(QDir::Files, QDir::Name)) {
    if (info.fileName().endsWith(".hist")) {
      QTreeWidgetItem *item = new QTreeWidgetItem();
      _tree->addTopLevelItem(item);
      item->setText(0, info.fileName());

      histogram_reader reader(info.filePath().toLocal8Bit().constData());
      auto names = reader.names();
      for (auto hname: names) {
        file_plot_source *src = new file_plot_source(info.filePath(), hname);
        QString name = info.fileName() + "#" + hname.c_str();

        QTreeWidgetItem *child = new QTreeWidgetItem(item);
        child->setText(0, hname.c_str());
        child->setData(0, Qt::UserRole, name);

        _data[name] = plot_data{ src, child, nullptr, QColor() };
      }
    }
  }
}

void multiplot_window::update_min(double min)
{
  _min = min;
  update_config();
  update_plots();
}

void multiplot_window::update_max(double max)
{
  _max = max;
  update_config();
  update_plots();
}

void multiplot_window::update_bins(int bins)
{
  _bins = bins;
  _config.axis = hist::linear_axis<double>(_min, _max, _bins);
  update_plots();
}

void multiplot_window::update_config()
{
  // Update the plot range
  double min = _min;
  double max = _max;
  for (auto &data: _data) {
    if (data.plottable != nullptr) {
      // Plot is shown
      data.source->minmax(min, max);
    }
  }
  _config.axis = hist::linear_axis<double>(min, max, _bins);
}

void multiplot_window::set_log_scale(bool log)
{
  _plot->yAxis->setScaleType(log ? QCPAxis::stLogarithmic : QCPAxis::stLinear);
  _plot->replot();
}
