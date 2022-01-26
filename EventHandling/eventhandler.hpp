#pragma once

#include "delegate.hpp"
#include "multicastdelegate.hpp"

namespace events
{

  template<typename ...Args>
  using EventHandler = delegates::Delegate<void, Args...>;

  template<typename ...Args>
  EventHandler<Args... > handler(ptr::FunctionPtr<void, Args...> function) {
    return EventHandler<Args...>(function);
  }

  template<typename Object, typename ...Args>
  EventHandler<Args... > handler(Object* obj, ptr::MethodPtr<Object, void, Args...> method) {
    return EventHandler<Args...>(obj, method);
  }

}
