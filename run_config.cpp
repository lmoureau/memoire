#include "run_config.h"

#include <iostream>

run_config::run_config(QWidget *parent) :
  QListWidget(parent)
{
  connect(this, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(item_changed()));
}

void run_config::add_lua_cut(const std::string &name, const std::string &code,
                             bool enable)
{
  _cuts.push_back(cut_info{nullptr, std::make_shared<lua_cut>(name, code), enable});
  QListWidgetItem *item = new QListWidgetItem(name.c_str(), this);
  item->setCheckState(enable ? Qt::Checked : Qt::Unchecked);
  item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
  emit config_changed();
}

void run_config::add_cut(const std::string &name, lambda_cut::lambda_function cut)
{
  add_cut(name, true, cut);
}

void run_config::add_cut(const std::string &name, bool enable, lambda_cut::lambda_function cut)
{
  _cuts.push_back(cut_info{std::make_shared<lambda_cut>(name, cut), nullptr, enable});
  QListWidgetItem *item = new QListWidgetItem(name.c_str(), this);
  item->setCheckState(enable ? Qt::Checked : Qt::Unchecked);
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
      if (it->c) {
        r.add(it->c);
      } else {
        r.add(it->lc);
      }
    }
  }
}
