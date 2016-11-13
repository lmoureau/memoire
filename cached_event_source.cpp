#include "cached_event_source.h"

cached_event_source::cached_event_source(event_source *in) :
  _in(in),
  _cache_position(-1),
  _cache_size(0),
  _cache_valid(false)
{}

bool cached_event_source::end()
{
  if (_cache_valid) {
    return _cache_position == _cache_size - 1;
  } else {
    bool end = _in->end();
    _cache_valid = end;
    return end;
  }
}

void cached_event_source::read()
{
  if (_cache_valid) {
    _cache_position++;
  } else {
    _in->read();
    _cache.push_back(_in->rec());
    _cache_position++;
    _cache_size++;
  }
}

event cached_event_source::rec()
{
  return _cache[_cache_position];
}

event cached_event_source::gen()
{
  return _cache[_cache_position];
}

void cached_event_source::reset()
{
  if (_cache_valid) {
    _cache_position = 0;
  } else {
    _in->reset();
    _cache.empty();
  }
}
