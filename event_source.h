#ifndef EVENT_SOURCE_H
#define EVENT_SOURCE_H

#include "event.h"

class event_source
{
public:
  virtual ~event_source() {}

  virtual bool end() = 0;
  virtual bool has_gen() { return true; }
  virtual bool has_rec() { return true; }
  virtual void read() = 0;
  virtual event rec() = 0;
  virtual event gen() = 0;
  virtual void reset() = 0;
};

#endif // EVENT_SOURCE_H
