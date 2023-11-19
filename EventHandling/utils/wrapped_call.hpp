#pragma once

#include <memory>

#include "argument_pack.hpp"
#include "invocable_element.hpp"
#include "result_store.hpp"

namespace eh {

namespace utils {

class WrappedCallBase {
 public:
  using Ptr = std::shared_ptr<WrappedCallBase>;
  using WPtr = std::weak_ptr<WrappedCallBase>;

  WrappedCallBase(WrappedCallBase&&) = delete;
  virtual ~WrappedCallBase() {}
  virtual void Perform() = 0;

 private:
 protected:
  WrappedCallBase() noexcept = default;
};

using WrappedCallBasePtr = WrappedCallBase::Ptr;
using WrappedCallBaseWPtr = WrappedCallBase::WPtr;

template <typename T>
class WrappedCall : public WrappedCallBase {
 public:
  WrappedCall(WrappedCall&&) = delete;
  bool HasValue() const { return m_result.HasValue(); }
  T Retrieve() { return m_result.Retrieve(); }
  decltype(auto) Get() const { return m_result.Get(); }
  std::future<T> GetFuture() { return std::move(m_result.GetFuture()); }

 protected:
  WrappedCall() noexcept = default;
  ResultStore<T>& GetResultStore() noexcept { return m_result; }

 private:
  ResultStore<T> m_result;
};

template <typename Ret, typename... Args>
class WrappedCallImpl final : public WrappedCall<Ret> {
 public:
  using Callable = std::shared_ptr<InvocableBase<Ret, Args...>>;

  explicit WrappedCallImpl(const Callable& callable, Args&&... args)
      : m_callable(callable), m_args(std::forward<Args>(args)...) {}

  void Perform() override {
    auto& result = this->GetResultStore();
    try {
      if constexpr (std::is_void_v<Ret>) {
        m_args.Apply(m_callable);
        result.SetValue();
      } else {
        result.SetValue(m_args.Apply(m_callable));
      }
    } catch (const std::exception&) {
      result.SetException(std::current_exception());
    }
  }

 private:
  std::weak_ptr<InvocableBase<Ret, Args...>> m_callable;
  ArgumentPack<Args...> m_args;
};

}  // namespace utils

}  // namespace eh
