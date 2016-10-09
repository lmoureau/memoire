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
  typedef basic_histogram<BASIC_HISTOGRAM_PARAMS> histogram_type;

  const axis_type axis = histogram.axis();

  typename histogram_type::const_iterator it = histogram.begin();
  typename histogram_type::const_iterator end = histogram.end();
  for (int i = 0; it != end; ++it, ++i) {
    graph->addData(axis.bin_center(i), *it);
  }
}

class histogram2d_plottable : public QCPAbstractPlottable
{
  Q_OBJECT

  histogram2d _histo;

public:
  explicit histogram2d_plottable(QCPAxis *x, QCPAxis *y,
                                 const histogram2d &histo) :
    QCPAbstractPlottable(x, y),
    _histo(histo)
  {}

  void clearData() { /* Nothing */ }
  double selectTest(const QPointF &pos, bool onlySelectable,
                    QVariant *details = nullptr) const;
  QCPRange getKeyRange(bool &foundRange,
                       SignDomain inSignDomain = sdBoth) const;
  QCPRange getValueRange(bool &foundRange,
                         SignDomain inSignDomain = sdBoth) const;

protected:
  void draw(QCPPainter *painter);
  void drawLegendIcon(QCPPainter *painter, const QRectF &rect) const;
};

} // namespace qt
} // namespace hist

#endif // HISTOGRAM_QT_H
