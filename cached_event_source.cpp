#include "cached_event_source.h"

cached_event_source::cached_event_source(event_source *in,
                                         std::size_t size_hint) :
  _in(in),
  _cache_position(-1),
  _cache_size(0),
  _cache_valid(false)
{
  if (size_hint > 0) {
    _cache.reserve(size_hint);
  }
}

bool cached_event_source::end()
{
  if (_cache_valid) {
    return _cache_position == _cache_size - 1;
  } else {
    bool end = _in->end();
    if (end) {
      _cache.shrink_to_fit();
      _cache_valid = true;
    }
    return end;
  }
}

void cached_event_source::fill_rec(sol::state &lua, sol::table &event)
{
  _in->fill_rec(lua, event);
}

void cached_event_source::prepare(sol::state &lua, sol::table &event)
{
  _in->prepare(lua, event);
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

const event &cached_event_source::rec()
{
  return _cache[_cache_position];
}

const event &cached_event_source::gen()
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
