#pragma once

#include "eventhandler.hpp"
#include "multicastdelegate.hpp"

namespace events {

  template<typename ...Args>
  class IEvent {

  public:
    void operator+= (const EventHandler<Args...>& handler) {
      addHandler(handler);
    }

    void operator-= (const EventHandler<Args...>& handler) {
      removeHandler(handler);
    }

  protected:
    virtual void addHandler(const EventHandler<Args...>& handler) = 0;
    virtual void removeHandler(const EventHandler<Args...>& handler) = 0;

  };

  template<typename ...Args>
  class Event: public IEvent<Args...> {

  public:
    Event(): _handlers() {}

    void operator() (Args...args) {
      _handlers(args...);
    }

  protected:
    void addHandler(const EventHandler<Args...>& handler) override {
      _handlers += handler;
    }

    void removeHandler(const EventHandler<Args...>& handler) override {
      _handlers -= handler;
    }

  private:
    delegates::MulticastDelegate<void, Args...> _handlers;

  };
}


