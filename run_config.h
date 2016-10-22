#ifndef RUN_CONFIG_H
#define RUN_CONFIG_H

#include <string>
#include <tuple>
#include <vector>

#include <QListWidget>

#include "run.h"

class run_config : public QListWidget
{
  Q_OBJECT

  struct cut_info {
    std::string name;
    run::cut_fct function;
    bool enabled;
  };
  std::vector<cut_info> _cuts;

public:
  explicit run_config(QWidget *parent = 0);

  void add_cut(const std::string &name, run::cut_fct cut);
  void add_cut(const std::string &name, bool enable, run::cut_fct cut);
  void fill_run(run &r) const;

signals:
  void config_changed() const;

private slots:
  void item_changed();
};

#endif // RUN_CONFIG_H
