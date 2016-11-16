#ifndef CACHED_EVENT_SOURCE_H
#define CACHED_EVENT_SOURCE_H

#include "event.h"
#include "event_source.h"

#include <vector>

class cached_event_source : public event_source
{
  event_source *_in;

  std::vector<event> _cache;
  std::size_t _cache_position;
  std::size_t _cache_size;
  bool _cache_valid;

public:
  explicit cached_event_source(event_source *in, std::size_t size_hint = 0);

  bool end();
  void read();
  void prepare(sol::state &lua);
  const event &rec();
  const event &gen();
  void reset();
};

#endif // CACHED_EVENT_SOURCE_H
