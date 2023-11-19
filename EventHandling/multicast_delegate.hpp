#pragma once

#include <list>
#include <mutex>
#include <algorithm>

#include "delegate.hpp"
#include "delegate_base.h"
#include "delegate_executor.hpp"

namespace eh {

namespace delegates {

namespace {

template <typename Ret, typename... Args>
struct MulticastDelegateCore {
  using DelegateShared = Delegate<Ret, Args...>;
  using Mutex = std::recursive_mutex;
  using Executor = DelegateExecutor<Ret, Args...>;

  std::list<DelegateShared> handlers;
  bool use_executor{false};
  Executor executor;
  mutable Mutex coreMutex;

  inline std::list<DelegateShared>::iterator find(
      const Delegate<Ret, Args...>& d) {
    return std::find(handlers.begin(), handlers.end(), d);
  }
};

}  // namespace

template <typename Ret, typename... Args>
class MulticastDelegate : public IDelegate<Ret, Args...> {

 public:
  MulticastDelegate() noexcept = default;
  MulticastDelegate(const MulticastDelegate& other) {
    std::lock_guard<Mutex> g(other.m_core.coreMutex);
    std::copy(other.m_core.handlers.begin(), other.m_core.handlers.last(),
              m_core.begin());
  }
  MulticastDelegate(MulticastDelegate&& other) noexcept {
    std::lock_guard<Mutex> g(other.m_core.coreMutex);
    m_core.handlers = std::move(other.m_core.handlers);
  }

  MulticastDelegate(const Delegate<Ret, Args...>&& d) : MulticastDelegate() {
    std::lock_guard<Mutex> g(m_core.coreMutex);
    m_core.handlers.push_back(d);
  }

  ~MulticastDelegate() { 
    std::lock_guard<Mutex> g(m_core.coreMutex);
    m_core.handlers.clear();
  }

  void SetUseExecutor(bool use_executor) {
    std::lock_guard<Mutex> g(m_core.coreMutex);
    m_core.use_executor = use_executor;
  }

  bool UseExecutor() const {
    std::lock_guard<Mutex> g(m_core.coreMutex);
    return m_core.use_executor;
  }

  Ret Invoke(Args&&... args) override {
    if (IsEmpty()) {
      throw DelegateException("Multicast Delegate is empty");
    }
    auto it = m_core.handlers.begin();
    for (; it != --m_core.handlers.end(); ++it) {
      std::lock_guard<Mutex> g(m_core.coreMutex);
      if (m_core.use_executor) {
        m_core.executor.Execute(*it, std::forward<Args>(args)...);
      } else {
        it->Invoke(std::forward<Args>(args)...);
      }
    }
    std::lock_guard<Mutex> g(m_core.coreMutex);
    if (m_core.use_executor) {
      return m_core.executor.Execute(*it, std::forward<Args>(args)...);
    } else {
      return it->Invoke(std::forward<Args>(args)...);
    }
    //return it->Invoke(std::forward<Args>(args)...);
    //return m_core.executor.Execute(*it, std::forward<Args>(args)...);
  }

  bool IsEmpty() const override {
    std::lock_guard<Mutex> g(m_core.coreMutex);
    return m_core.handlers.empty();
  }

  void Reset() override {
    std::lock_guard<Mutex> g(m_core.coreMutex);
    m_core.handlers.clear();
  }

  void SetInvokeType(InvokeType /*type*/) override { }
  void SetThreadId(std::thread::id /*id*/) override { }

  MulticastDelegate<Ret, Args...>& operator+=(const Delegate<Ret, Args...>& d) {
    std::lock_guard<Mutex> g(m_core.coreMutex);
    m_core.handlers.push_back(d);
    return *this;
  }

  MulticastDelegate<Ret, Args...>& operator+=(Delegate<Ret, Args...>&& d) {
    std::lock_guard<Mutex> g(m_core.coreMutex);
    m_core.handlers.push_back(std::move(d));
    return *this;
  }

  MulticastDelegate<Ret, Args...>& operator-=(const Delegate<Ret, Args...>& d) {
    std::lock_guard<Mutex> g(m_core.coreMutex);
    auto it = m_core.find(d);
    if (it != m_core.handlers.end()) {
      m_core.handlers.erase(it);
    }
    return *this;
  }

 private:
  MulticastDelegateCore<Ret, Args...> m_core;

  using Type = MulticastDelegate<Ret, Args...>;
  using Mutex = MulticastDelegateCore<Ret, Args...>::Mutex;

 protected:
  bool Equals(const IDelegate<Ret, Args...>& other) const override {
    const Type* other_casted = dynamic_cast<const Type*>(&other);
    if (other_casted == nullptr) {
      return false;
    }
    std::scoped_lock<Mutex, Mutex> lck{m_core.coreMutex, other_casted->m_core.coreMutex};
    return m_core.handlers == other_casted->m_core.handlers;
  }
};

template <typename Ret, typename... Args>
MulticastDelegate<Ret, Args...> operator+(const Delegate<Ret, Args...>& ld,
                                          const Delegate<Ret, Args...>& rd) {
  MulticastDelegate<Ret, Args...> res;
  res += ld;
  res += rd;
  return res;
}

}  // namespace delegates

}  // namespace eh
