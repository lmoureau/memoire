#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <iostream>
#include <stdexcept>
#include <vector>

namespace hist
{

template<typename _axis_type_>
class linear_axis
{
public:
  using axis_type = _axis_type_;

private:
  axis_type _begin, _end;
  axis_type _bin_width;

public:
  explicit linear_axis(const axis_type &begin, const axis_type &end,
                       int bin_count);

  int operator() (const axis_type &data) const
  { return (data - _begin) / _bin_width; }

  int bin_count() const { return (_end - _begin) / _bin_width; }
  axis_type bin_end(int bin) const { return _begin + bin * _bin_width; }
  axis_type bin_center(int bin) const
  { return _begin + (bin + .5) * _bin_width; }
  axis_type bin_start(int bin) const { return _begin + (bin + 1) * _bin_width; }
};

template<typename _bin_type_>
class out_of_range_record
{
public:
  using bin_type = _bin_type_;

private:
  bin_type _underflow, _overflow;

public:
  void bin_overflow(bin_type weight) { _overflow += weight; }
  void bin_underflow(bin_type weight) { _underflow += weight; }
};

template<typename _axis_type_>
linear_axis<_axis_type_>::linear_axis(const axis_type &begin,
                                      const axis_type &end, int bin_count) :
  _begin(begin),
  _end(end),
  _bin_width((_end - _begin) / bin_count)
{}

#ifdef BASIC_HISTOGRAM_PARAMS_DEF
  #error Please don't define BASIC_HISTOGRAM_PARAMS_DEF
#endif
#ifdef BASIC_HISTOGRAM_PARAMS
  #error Please don't define BASIC_HISTOGRAM_PARAMS
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
  using axis_type = _axis_type_;
  using binned_type = _binned_type_;
  using bin_type = _bin_type_;
  using out_of_range_type = _out_of_range_type_;
  using vector_type = std::vector<bin_type>;

  using const_iterator = typename vector_type::const_iterator;
  using iterator = typename vector_type::iterator;

private:
  axis_type _axis;
  out_of_range_type _out_of_range;
  vector_type _data;

public:
  template<typename... _axis_args_>
  explicit basic_histogram(_axis_args_... args);
  virtual ~basic_histogram() {}

  void bin(const binned_type &value) { bin(value, binned_type(1)); }
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

using histogram = basic_histogram<double, double>;

} // namespace hist

#endif // HISTOGRAM_H
