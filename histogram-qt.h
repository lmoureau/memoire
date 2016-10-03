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
  typedef _axis_type_ axis_type;
  typedef _bin_type_ bin_type;
  typedef _binned_type_ binned_type;

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
