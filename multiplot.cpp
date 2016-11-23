#include <iostream>

#include <QApplication>
#include <QTreeWidget>
#include <QDir>

#include "histogram_reader.h"
#include "multiplot_window.h"
#include "plot_source.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  multiplot_window *win = new multiplot_window();
  win->setWindowTitle("Be He");
  win->show();

  return app.exec();
}
