#include "multiplot_window.h"

#include <cassert>

#include <QCheckBox>
#include <QDir>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QSpinBox>
#include <QSplitter>
#include <QSignalMapper>
#include <QTreeWidget>
#include <QVBoxLayout>

#include "histogram_reader.h"
#include "lua_plot_source.h"
#include "qcustomplot.h"

multiplot_window::multiplot_window() :
  _plot_list_widget(new QListWidget),
  _tree(new QTreeWidget),
  _plot(new QCustomPlot),
  _vbox(new QVBoxLayout),
  _min(0), _max(2), _bins(100),
  _config(plot_source::config{ hist::linear_axis<double>(_min, _max, _bins) }),
  _function(nullptr),
  _function_edit(new QLineEdit)
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

  _vbox->addWidget(_function_edit);
  _function_edit->setPlaceholderText(tr("Enter expression..."));
  connect(_function_edit, SIGNAL(editingFinished()),
          this, SLOT(update_plots()));

  splitter->addWidget(_plot_list_widget);

  update_config();
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
  if (data.source == nullptr) {
    data.source = new file_plot_source(data.path, data.name);
  }
  data.plottable = data.source->plot(_plot->xAxis, _plot->yAxis, _config);
  data.plottable->setName(data.display_name);
  data.color = next_color();
  data.plottable->setPen(data.color);
  data.item->setData(0, Qt::TextColorRole, data.color);
  _plot->addPlottable(data.plottable);
  _plot->xAxis->rescale();
  _plot->yAxis->rescale();
  _plot->replot();

  data.list_item = new QListWidgetItem(data.display_name);
  data.list_item->setData(Qt::TextColorRole, data.color);
  data.list_item->setFlags(Qt::ItemIsEditable | data.list_item->flags());
  _plot_list_widget->addItem(data.list_item);

  data.index = _plots.size();
  _plots.append(data);

  update_lua_plot();
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

  for (int i = 0; i < _plot_list_widget->count(); ++i) {
    if (_plot_list_widget->item(i) == data.list_item) {
      _plot_list_widget->takeItem(i);
      delete data.list_item;
      data.list_item = nullptr;
      break;
    }
  }

  _plots.remove(data.index);
  update_lua_plot();
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
    }
  }

  update_display_names();
  update_lua_plot();

  _plot->xAxis->rescale();
  _plot->yAxis->rescale();
  _plot->replot();
}

void multiplot_window::update_display_names()
{
  for (int i = 0; i < _plots.size(); ++i) {
    _plots[i].display_name = _plot_list_widget->item(i)->text();
  }
}

void multiplot_window::update_lua_plot()
{
  lua_plot_source src;
  src.set_expression(_function_edit->text());
  for (auto &info : _plots) {
    src.add_global(info.display_name, info.source);
  }
  if (_function != nullptr) {
    _plot->removePlottable(_function);
  }
  _function = src.plot(_plot->xAxis, _plot->yAxis, _config);
  if (_function != nullptr) {
    _function->setName(_function_edit->text());
    _function->setPen(QColor(Qt::darkYellow));
    _plot->addPlottable(_function);
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

  _minbox = new QDoubleSpinBox;
  _minbox->setMinimum(std::numeric_limits<double>::lowest());
  _minbox->setMaximum(std::numeric_limits<double>::max());
  _minbox->setValue(_min);
  label->setBuddy(_minbox);
  connect(_minbox, SIGNAL(valueChanged(double)), this, SLOT(update_min(double)));
  layout->addWidget(_minbox);

  label = new QLabel(tr("M&ax:"));
  layout->addWidget(label);

  _maxbox = new QDoubleSpinBox;
  _maxbox->setMinimum(std::numeric_limits<double>::lowest());
  _maxbox->setMaximum(std::numeric_limits<double>::max());
  _maxbox->setValue(_max);
  label->setBuddy(_maxbox);
  connect(_maxbox, SIGNAL(valueChanged(double)), this, SLOT(update_max(double)));
  layout->addWidget(_maxbox);

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
        QString name = info.fileName() + "#" + hname.c_str();

        QTreeWidgetItem *child = new QTreeWidgetItem(item);
        child->setText(0, hname.c_str());
        child->setData(0, Qt::UserRole, name);

        _data[name] = plot_data{
          info.filePath(),
          hname,
          name,
          nullptr,
          child,
          nullptr,
          nullptr,
          QColor(),
          0
        };
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
  // Update the boxes range
  _minbox->setMaximum(_max - 0.01);
  _maxbox->setMinimum(_min + 0.01);

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
