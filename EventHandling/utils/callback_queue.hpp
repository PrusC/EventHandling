#pragma once

#include <list>
#include <memory>
#include <mutex>

#include "callback_queue_base.hpp"

namespace eh {

namespace utils {

class NoopCallbackQueue : public ICallbackQueue {
 public:
  NoopCallbackQueue() = default;

  void addCallback(const WrappedCallBasePtr& callback) override {
    std::lock_guard<std::mutex> g(m_mutex);
    m_callback = callback;
    //m_callback->Perform();
  }

  WrappedCallBasePtr Get() override {
    std::lock_guard<std::mutex> g(m_mutex);
    return std::move(m_callback);
  }

  bool empty() const override {
    std::lock_guard<std::mutex> g(m_mutex);
    return m_callback == nullptr;
  }

 private:
  mutable std::mutex m_mutex;
  WrappedCallBasePtr m_callback;
};

class CallBackQueue : public ICallbackQueue {
 public:
  CallBackQueue() = default;

  void addCallback(const WrappedCallBasePtr& callback) override {
    std::lock_guard<std::mutex> g(m_mutex);
    m_callbacks.push_back(callback);
  }

  bool empty() const override {
    std::lock_guard<std::mutex> g(m_mutex);
    return m_callbacks.empty();
  }

  WrappedCallBasePtr Get() override {
    std::lock_guard<std::mutex> g(m_mutex);
    if (m_callbacks.empty()) {
      return nullptr;
    }
    WrappedCallBasePtr callback = m_callbacks.front();
    m_callbacks.pop_front();
    return callback;
  }

 private:
  mutable std::mutex m_mutex;

  std::list<WrappedCallBaseWPtr> m_weak_callbacks;
  std::list<WrappedCallBasePtr> m_callbacks;
};

}  // namespace utils

}  // namespace eh