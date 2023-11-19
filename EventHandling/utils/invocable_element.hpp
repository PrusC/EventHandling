#pragma once

#include <functional>
#include <memory>
#include <type_traits>

#include "ptr.hpp"
#include "helper.hpp"

namespace eh {

namespace utils {

template <typename Ret, typename... Args>
class InvocationElementFactory;

template <typename Ret, typename... Args>
class InvocableBase : public std::enable_shared_from_this<InvocableBase<Ret, Args...>> {
 public:
  using Ptr = std::shared_ptr<InvocableBase<Ret, Args...>>;
  using WPtr = std::weak_ptr<InvocableBase<Ret, Args...>>;

  virtual ~InvocableBase() {}

  bool operator==(const InvocableBase& other) const { return equals(other); }

  bool operator!=(const InvocableBase& other) const {
    return !(*this == other);
  }

  Ret operator()(Args&&... args) { return invoke(std::forward<Args>(args)...); }
  virtual Ret invoke(Args&&... args) = 0;

  Ptr Copy() { return this->shared_from_this(); }
  WPtr WeakCopy() { return this->weak_from_this(); }

 protected:
  virtual bool equals(const InvocableBase<Ret, Args...>& other) const = 0;
};

template <typename Ret, typename... Args>
using InvocableBasePtr = InvocableBase<Ret, Args...>::Ptr;

template <typename Ret, typename... Args>
using InvocableBaseWPtr = InvocableBase<Ret, Args...>::WPtr;

template <typename Ret, typename... Args>
class InvocationElementFactory final {
 private:
  class FunctionWrapper final : public InvocableBase<Ret, Args...> {
    using Function = ptr::FunctionPtr<Ret, Args...>;

   public:
    FunctionWrapper(Function function) : m_func(function) {}

    Ret invoke(Args&&... args) override {
      return m_func(std::forward<Args>(args)...);
    }

   private:
    bool equals(const InvocableBase<Ret, Args...>& other) const override {
      const FunctionWrapper* _other =
          dynamic_cast<const FunctionWrapper*>(&other);
      return _other != nullptr && m_func == _other->m_func;
    }

    ptr::FunctionPtr<Ret, Args...> m_func;
  };

  template <typename Object, typename Method>
  class MethodWrapper final : public InvocableBase<Ret, Args...> {
   public:
    MethodWrapper(Object* object, Method method)
        : m_object(object), m_method(method) {}

    Ret invoke(Args&&... args) override {
      return std::invoke(m_method, m_object, std::forward<Args>(args)...);
      //return m_object->m_method(std::forward<Args>...);
    }

   private:
    bool equals(const InvocableBase<Ret, Args...>& other) const override {
      const MethodWrapper* _other = dynamic_cast<const MethodWrapper*>(&other);
      return _other != nullptr && m_method == _other->m_method && m_object ==
                 _other->m_object;
    }

    Object* m_object;
    Method m_method;
  };

  template <typename Callable>
  class CallableWrapper final : public InvocableBase<Ret, Args...> {
   public:
    CallableWrapper(const Callable& callable)
        : m_callable(callable) {}

    CallableWrapper(Callable&& callable)
        : m_callable(std::move(callable)) {}

    Ret invoke(Args&&... args) override {
      return m_callable(std::forward<Args>(args)...);
    }

   private:

    bool equals(const InvocableBase<Ret, Args...>& other) const override {
      if constexpr (is_equality_comparable_v<Callable>) {
        const CallableWrapper* _other =
            dynamic_cast<const CallableWrapper*>(&other);
        return _other != nullptr && m_callable == _other->m_callable;
      } else {
        return false;
      }
    }

    Callable m_callable;

  };

 public:
  constexpr static std::shared_ptr<InvocableBase<Ret, Args...>> create(
      const ptr::FunctionPtr<Ret, Args...>& func) {
    return std::make_shared<FunctionWrapper>(func);
  }

  template <typename Object, typename Method,
            typename = std::enable_if_t<
                std::is_invocable_r_v<Ret, Method, Object, Args...>>>
  constexpr static std::shared_ptr<InvocableBase<Ret, Args...>> create(
      Object* object, Method method) {
    return std::make_shared<MethodWrapper<Object, Method>>(object, method);
  }

  template <typename Callable,
            typename =
                std::enable_if_t<std::is_invocable_r_v<Ret, Callable, Args...>>>
  constexpr static std::shared_ptr<InvocableBase<Ret, Args...>> create(
      const Callable& callable) {
    return std::make_shared<CallableWrapper<std::remove_reference_t<Callable>>>(
        callable);
  }

  template <typename Callable,
            typename =
                std::enable_if_t<std::is_invocable_r_v<Ret, Callable, Args...>>>
  constexpr static std::shared_ptr<InvocableBase<Ret, Args...>> create(
      Callable&& callable) {
    return std::make_shared<CallableWrapper<std::remove_reference_t<Callable>>>(
        std::move(callable));
  }
};

}  // namespace detail

}  // namespace eh
