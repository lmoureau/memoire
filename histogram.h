#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace hist
{

template<typename _axis_type_>
class linear_axis
{
public:
  typedef _axis_type_ axis_type;

private:
  axis_type _begin, _end;
  axis_type _bin_width;

public:
  explicit linear_axis(const axis_type &begin, const axis_type &end,
                       int bin_count);

  int operator() (const axis_type &data) const
  { return std::floor(data / _bin_width - _begin / _bin_width); }

  int bin_count() const { return _end / _bin_width - _begin / _bin_width; }
  axis_type bin_end(int bin) const { return _begin + bin * _bin_width; }
  axis_type bin_center(int bin) const
  { return _begin + (bin + .5) * _bin_width; }
  axis_type bin_start(int bin) const { return _begin + (bin + 1) * _bin_width; }
};

template<typename _bin_type_>
class out_of_range_discard
{
public:
  typedef _bin_type_ bin_type;

  void bin_overflow(bin_type) {}
  void bin_underflow(bin_type) {}
};

template<typename _bin_type_>
class out_of_range_record
{
public:
  typedef _bin_type_ bin_type;

private:
  bin_type _underflow, _overflow;

public:
  void bin_overflow(bin_type weight) { _overflow += weight; }
  void bin_underflow(bin_type weight) { _underflow += weight; }

  bin_type underflow() const { return _underflow; }
  bin_type overflow() const { return _overflow; }
};

template<typename _axis_type_>
linear_axis<_axis_type_>::linear_axis(const axis_type &begin,
                                      const axis_type &end, int bin_count) :
  _begin(begin),
  _end(end),
  _bin_width(_end / bin_count - _begin / bin_count)
{}

#ifdef BASIC_HISTOGRAM_PARAMS_DEF
  #error Please do not define BASIC_HISTOGRAM_PARAMS_DEF
#endif
#ifdef BASIC_HISTOGRAM_PARAMS
  #error Please do not define BASIC_HISTOGRAM_PARAMS
#endif

#define BASIC_HISTOGRAM_PARAMS_DEF \
  template<                                                                    \
    typename _binned_type_,                                                    \
    typename _bin_type_,                                                       \
    typename _axis_type_,                                                      \
    typename _out_of_range_type_                                               \
  >

#define BASIC_HISTOGRAM_PARAMS \
  _binned_type_,                                                               \
  _bin_type_,                                                                  \
  _axis_type_,                                                                 \
  _out_of_range_type_

/**
 * \brief Base class for histograms
 *
 * Histograms are primarily used to record the distribution of some value in a
 * dataset.
 *
 * \tparam _binned_type_  The type of the data to record the distribution of.
 * \tparam _bin_type_     The type in which the distribution shall be recorded.
 * \tparam _axis_type_    The type of axis to use. The axis defines the bin
 *                        boundaries.
 * \tparam _out_of_range_type_ A type allowing to customize the behaviour on
 *                        overflow/underflow.
 */
template<
  typename _binned_type_,
  typename _bin_type_,
  typename _axis_type_ = linear_axis<_binned_type_>,
  typename _out_of_range_type_ = out_of_range_record<_bin_type_>
>
class basic_histogram
{
public:
  typedef _axis_type_ axis_type;
  typedef _binned_type_ binned_type;
  typedef _bin_type_ bin_type;
  typedef _out_of_range_type_ out_of_range_type;
  typedef std::vector<bin_type> vector_type;

  typedef typename vector_type::const_iterator const_iterator;
  typedef typename vector_type::iterator iterator;

private:
  axis_type _axis;
  out_of_range_type _out_of_range;
  vector_type _data;

public:
  template<typename... _axis_args_>
  explicit basic_histogram(_axis_args_... args);
  virtual ~basic_histogram() {}

  void bin(const binned_type &value) { bin(value, bin_type(1)); }
  void bin(const binned_type &value, const bin_type &weight);

  const_iterator begin() const { return _data.cbegin(); }
  const_iterator cbegin() const { return _data.cbegin(); }
  const_iterator cend() const { return _data.cend(); }
  const_iterator end() const { return _data.cend(); }

  iterator begin() { return _data.begin(); }
  iterator end() { return _data.end(); }

  axis_type axis() const { return _axis; }
  vector_type data() const { return _data; }
  out_of_range_type out_of_range() const { return _out_of_range; }
};

/**
 * \brief Constructor for basic_histogram
 *
 * Any argument passed here is directly forwarded to the axis constructor.
 *
 * \param args Arguments to pass to the axis constructor.
 */
BASIC_HISTOGRAM_PARAMS_DEF
template<typename... _axis_args_>
basic_histogram<BASIC_HISTOGRAM_PARAMS>::basic_histogram(_axis_args_... args) :
  _axis(args...),
  _data(_axis.bin_count())
{}

/**
 * \brief Increment the bin corresponding to \arg value.
 *
 * \param value   The bin to increment.
 * \param weight  A weight .
 */
BASIC_HISTOGRAM_PARAMS_DEF
void basic_histogram<BASIC_HISTOGRAM_PARAMS>::bin(const binned_type &value,
                                                  const bin_type &weight)
{
  int bin = _axis(value);
  if (bin < 0) {
    _out_of_range.bin_underflow(weight);
  } else if (bin >= (int) _data.size()) {
    _out_of_range.bin_overflow(weight);
  } else {
    _data[bin] += weight;
  }
}

typedef basic_histogram<double, double> histogram;

template<typename _binned_type_1_, typename _binned_type_2_>
class basic_bin2d
{
public:
  using binned_type_1 = _binned_type_1_;
  using binned_type_2 = _binned_type_2_;

  binned_type_1 x;
  binned_type_2 y;

  explicit basic_bin2d(const binned_type_1 &x, const binned_type_2 &y) :
    x(x), y(y)
  {}
};

using bin2d = basic_bin2d<double, double>;

template<
  typename _axis_type_1_,
  typename _axis_type_2_>
class axis2d
{
public:
  using axis_type_1 = _axis_type_1_;
  using axis_type_2 = _axis_type_2_;
  using bin_type = basic_bin2d<typename _axis_type_1_::axis_type,
                               typename _axis_type_2_::axis_type>;

  axis_type_1 x;
  axis_type_2 y;

public:
  explicit axis2d(const axis_type_1 &axis1, const axis_type_2 &axis2) :
    x(axis1), y(axis2)
  {}

  int bin_count() const { return x.bin_count() * y.bin_count(); }
  int operator() (const bin_type &value);
};

template<typename _axis_type_1_, typename _axis_type_2_>
int axis2d<_axis_type_1_, _axis_type_2_>::operator() (
    const axis2d<_axis_type_1_, _axis_type_2_>::bin_type &value)
{
  return x(value.x) * y.bin_count() + y(value.y);
}

template<typename _out_of_range_type_1_, typename _out_of_range_type_2_>
class out_of_range2d
{};

template<
  typename _binned_type_1_,
  typename _binned_type_2_,
  typename _bin_type_,
  typename _axis_type_1_ = linear_axis<_binned_type_1_>,
  typename _axis_type_2_ = linear_axis<_binned_type_2_>,
  typename _out_of_range_type_ = out_of_range_discard<_bin_type_>
>
using basic_histogram2d = basic_histogram<
  basic_bin2d<_binned_type_1_, _binned_type_2_>,
  _bin_type_,
  axis2d<_axis_type_1_, _axis_type_2_>,
  _out_of_range_type_>;

using histogram2d = basic_histogram2d<double, double, double>;

} // namespace hist

#endif // HISTOGRAM_H
