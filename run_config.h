#ifndef RUN_CONFIG_H
#define RUN_CONFIG_H

#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include <QListWidget>

#include "cut.h"
#include "run.h"

class run_config : public QListWidget
{
  Q_OBJECT

  struct cut_info {
    std::shared_ptr<cut> c;
    std::shared_ptr<lua_cut> lc;
    bool enabled;
  };
  std::vector<cut_info> _cuts;

public:
  explicit run_config(QWidget *parent = 0);

  void add_lua_cut(const std::string &name, const std::string &code,
                   bool enable = true);
  void add_cut(const std::string &name, lambda_cut::lambda_function cut);
  void add_cut(const std::string &name, bool enable, lambda_cut::lambda_function cut);
  void fill_run(run &r) const;

signals:
  void config_changed() const;

private slots:
  void item_changed();
};

#endif // RUN_CONFIG_H
