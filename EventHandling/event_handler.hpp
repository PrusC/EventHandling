#pragma once

#include <type_traits>

#include "delegate.hpp"
#include "multicast_delegate.hpp"

namespace eh {

namespace events {

template <typename... Args>
using EventHandler = delegates::Delegate<void, Args...>;


template<typename... Args>
using EventHandlersStorage = delegates::MulticastDelegate<void, Args...>;


template <typename... Args>
EventHandler<Args...> handler(ptr::FunctionPtr<void, Args...> func) {
  return delegates::delegate<void, Args...>(func);
}

template <typename... Args, typename Obj, typename Method,
          typename = std::enable_if_t<
              std::is_invocable_r_v<void, Method, Obj, Args...>>>
EventHandler<Args...> handler(Obj* object, Method method) {
  return delegates::delegate<void, Args..., Obj, Method>(object, method);
}

template <
    typename... Args, typename Callable,
    typename = std::enable_if_t<std::is_invocable_r_v<void, Callable, Args...>>>
EventHandler<Args...> handler(Callable&& callable) {
  return delegates::delegate<void, Args..., Callable>(
      std::forward<Callable>(callable));
}

}  // namespace handlers

}  // namespace eh
