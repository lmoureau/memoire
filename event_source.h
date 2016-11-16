#ifndef EVENT_SOURCE_H
#define EVENT_SOURCE_H

class event;

namespace sol
{
  class state;
}

class event_source
{
public:
  virtual ~event_source() {}

  virtual bool end() = 0;
  virtual bool has_gen() { return true; }
  virtual bool has_rec() { return true; }
  virtual void prepare(sol::state &lua) {};
  virtual void read() = 0;
  /// The returned reference has to remain valid until the next call to read()
  virtual void fill_rec(sol::state &lua) {};
  virtual const event &rec() = 0;
  /// The returned reference has to remain valid until the next call to read()
  virtual const event &gen() = 0;
  virtual void reset() = 0;
};

#endif // EVENT_SOURCE_H
