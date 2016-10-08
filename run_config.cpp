#include "run_config.h"

#include <iostream>

run_config::run_config(QWidget *parent) :
  QListWidget(parent)
{
  connect(this, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(item_changed()));
}

void run_config::add_cut(const std::string &name, run::cut_fct cut)
{
  _cuts.push_back(cut_info{name, cut, true});
  QListWidgetItem *item = new QListWidgetItem(name.c_str(), this);
  item->setCheckState(Qt::Checked);
  item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
  emit config_changed();
}

void run_config::item_changed()
{
  for (unsigned i = 0; i < _cuts.size(); ++i) {
    _cuts[i].enabled = (item(i)->checkState() == Qt::Checked);
  }
  emit config_changed();
}

void run_config::fill_run(run &r) const
{
  for (std::vector<cut_info>::const_iterator it = _cuts.begin();
       it != _cuts.end(); ++it) {
    if (it->enabled) {
      r.add_cut(it->name, it->function);
    }
  }
}
