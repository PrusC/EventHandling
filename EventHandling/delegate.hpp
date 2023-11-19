#pragma once

#include <exception>
#include <functional>
#include <memory>
#include <stdexcept>
#include <type_traits>

#include "delegate_base.h"
#include "utils\invocable_element.hpp"
#include "utils\ptr.hpp"
#include "utils\wrapped_call.hpp"

namespace eh {

namespace delegates {


template <typename Ret, typename... Args>
class DelegateExecutor;

template <typename Ret, typename... Args>
class Delegate : public IDelegate<Ret, Args...> {
  using Type = Delegate<Ret, Args...>;

 public:
  Delegate()
      : m_invocable(nullptr),
        m_invoke_type(InvokeType::Auto),
        m_thread_id(std::this_thread::get_id()) {}
  Delegate(const Delegate& other)
      : m_invocable(other.m_invocable),
        m_invoke_type(other.m_invoke_type),
        m_thread_id(other.m_thread_id) {}
  Delegate(Delegate&& other)
      : m_invocable(std::move(other.m_invocable)),
        m_invoke_type(other.m_invoke_type),
        m_thread_id(other.m_thread_id) {}

  bool IsEmpty() const override { return m_invocable == nullptr; }
  void Reset() override { m_invocable.reset(); }

  void SetInvokeType(InvokeType type) override { m_invoke_type = type; }
  void SetThreadId(std::thread::id id) override { m_thread_id = id; }

  Ret Invoke(Args&&... args) {
    if (IsEmpty()) {
      throw DelegateException("Callable object is empty");
    }
    return m_invocable->invoke(std::forward<Args>(args)...);
  }

  bool IsPartner(const Delegate& other) const {
    if (IsEmpty() || other.IsEmpty()) {
      return false;
    }
    return m_invocable == other.m_invocable;
  }

  template <typename Ret0, typename... Args0>
  friend Delegate<Ret0, Args0...> delegate(
      ptr::FunctionPtr<Ret0, Args0...> func);

  template <typename Ret0, typename... Args0, typename Obj, typename Method,
            typename>
  friend Delegate<Ret0, Args0...> delegate(Obj* object, Method method);

  template <typename Ret0, typename... Args0, typename Callable, typename>
  friend Delegate<Ret0, Args0...> delegate(Callable&& callable);

  template <typename Ret0, typename... Args0>
  friend utils::WrappedCallBasePtr WrappDelegateInvoke(
      const Delegate<Ret0, Args0...>& d, Args0&&... args);

  friend class DelegateExecutor<Ret, Args...>;

  //friend class DelegateExecutor<Ret, Args...>;

 private:
  explicit Delegate(const utils::InvocableBasePtr<Ret, Args...>& invocable)
      : m_invocable(invocable),
        m_invoke_type(InvokeType::Auto),
        m_thread_id(std::this_thread::get_id()) {}

  utils::InvocableBasePtr<Ret, Args...> m_invocable;
  InvokeType m_invoke_type{InvokeType::Auto};
  std::thread::id m_thread_id;

 protected:
  bool Equals(const IDelegate<Ret, Args...>& other) const {
    const Type* other_casted = dynamic_cast<const Type*>(&other);
    if (other_casted == nullptr) {
      return false;
    }
    if (IsPartner(*other_casted)) {
      return true;
    }
    return *m_invocable == *(other_casted->m_invocable);
  }
};

template <typename Ret, typename... Args>
Delegate<Ret, Args...> delegate(ptr::FunctionPtr<Ret, Args...> func) {
  return Delegate<Ret, Args...>(
      utils::InvocationElementFactory<Ret, Args...>::create(func));
}

template <typename Ret, typename... Args, typename Obj, typename Method,
          typename = std::enable_if_t<
              std::is_invocable_r_v<Ret, Method, Obj, Args...>>>
Delegate<Ret, Args...> delegate(Obj* object, Method method) {
  return Delegate<Ret, Args...>(
      utils::InvocationElementFactory<Ret, Args...>::create(object, method));
}

template <
    typename Ret, typename... Args, typename Callable,
    typename = std::enable_if_t<std::is_invocable_r_v<Ret, Callable, Args...>>>
Delegate<Ret, Args...> delegate(Callable&& callable) {
  return Delegate<Ret, Args...>(
      utils::InvocationElementFactory<Ret, Args...>::create(
          std::forward<Callable>(callable)));
}

template <typename Ret, typename... Args>
utils::WrappedCallBasePtr WrappDelegateInvoke(const Delegate<Ret, Args...>& d,
                                              Args&&... args) {
  return std::make_shared<utils::WrappedCallImpl<Ret, Args...>>(
      d.m_invocable, std::forward<Args>(args)...);
}

}  // namespace delegates

}  // namespace eh
