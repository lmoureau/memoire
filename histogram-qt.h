#ifndef HISTOGRAM_QT_H
#define HISTOGRAM_QT_H

#include "histogram.h"

#include <qcustomplot.h>

namespace hist {
namespace qt {

BASIC_HISTOGRAM_PARAMS_DEF
void set_graph_data(QCPGraph *graph,
                    const basic_histogram<BASIC_HISTOGRAM_PARAMS> &histogram);

BASIC_HISTOGRAM_PARAMS_DEF
void set_graph_data(QCPGraph *graph,
                    const basic_histogram<BASIC_HISTOGRAM_PARAMS> &histogram)
{
  using axis_type = _axis_type_;
  using bin_type = _bin_type_;
  using binned_type = _binned_type_;

  const axis_type axis = histogram.axis();

  int i = 0;
  for (const binned_type &bin : histogram) {
    graph->addData(axis.bin_center(i), bin);
    i++;
  }
}

} // namespace qt
} // namespace hist

#endif // HISTOGRAM_QT_H
