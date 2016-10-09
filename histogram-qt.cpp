#include "histogram-qt.h"

namespace hist {
namespace qt {

double histogram2d_plottable::selectTest(const QPointF &pos,
                                         bool onlySelectable,
                                         QVariant *details) const
{
  return -1;
}

QCPRange histogram2d_plottable::getKeyRange(
    bool &foundRange,
    histogram2d_plottable::SignDomain inSignDomain) const
{
  foundRange = true;
  const linear_axis<double> &key_axis = _histo.axis().x;
  return QCPRange(key_axis.bin_start(0),
                  key_axis.bin_start(key_axis.bin_count()));
}

QCPRange histogram2d_plottable::getValueRange(
    bool &foundRange,
    histogram2d_plottable::SignDomain inSignDomain) const
{
  foundRange = true;
  const linear_axis<double> &val_axis = _histo.axis().y;
  return QCPRange(val_axis.bin_start(0),
                  val_axis.bin_start(val_axis.bin_count()));
}

void histogram2d_plottable::draw(QCPPainter *painter)
{
  QBrush brush = mainBrush();
  const linear_axis<double> &key_axis = _histo.axis().x;
  const linear_axis<double> &val_axis = _histo.axis().y;

  const histogram2d::vector_type &data = _histo.data();

  double max = -1.0e10;
  double min = 1.0e10;
  for (double val : data) {
    if (val > max) {
      max = val;
    }
    if (val < min) {
      min = val;
    }
  }

  for (int x = 0; x < key_axis.bin_count(); ++x) {
    double lower_x = key_axis.bin_start(x);
    double upper_x = key_axis.bin_end(x);
    for (int y = 0; y < val_axis.bin_count(); ++y) {
      double lower_y = key_axis.bin_start(y);
      double upper_y = key_axis.bin_end(y);
      QPointF topLeft = coordsToPixels(lower_x, upper_y);
      QPointF bottomRight = coordsToPixels(upper_x, lower_y);
      QRectF rect(topLeft, bottomRight);

      int bin = x * val_axis.bin_count() + y;
      // We know val \in [-1, 0]
      double val = (data[bin] - min) / (max - min) - 1;
      painter->setOpacity(std::sqrt(1 - val * val * val * val));
      painter->fillRect(rect, brush);
    }
  }
  painter->setOpacity(1);
}

void histogram2d_plottable::drawLegendIcon(QCPPainter *painter,
                                           const QRectF &rect) const
{
  painter->fillRect(rect, mainBrush());
}

} // namespace qt
} // namespace hist
