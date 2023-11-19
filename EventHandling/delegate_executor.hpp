#pragma once

#include <future>
#include <memory>
#include <thread>

#include "delegate.hpp"
#include "delegate_internal_executor.hpp"
#include "delegate_invoke_type.h"
#include "thread.hpp"

namespace eh {

namespace delegates {

template <typename Ret, typename... Args>
class DelegateExecutor {
 private:
  using DefaultInternalExecutor = DefaultInternalExecutor<Ret, Args...>;
  using QueuedInternalExecutor = QueuedInternalExecutor<Ret, Args...>;

 public:
  using ExecutedType = Delegate<Ret, Args...>;

  DelegateExecutor()
      : m_use_event_system(false), m_default_executor(), m_executors() {}

  DelegateExecutor(bool use_event_system)
      : m_use_event_system(use_event_system),
        m_default_executor(),
        m_executors() {}

  DelegateExecutor(const DelegateExecutor& other)
      : m_use_event_system(other.m_use_event_system),
        m_default_executor(other.m_default_executor),
        m_executors(other.m_executors) {}

  DelegateExecutor(DelegateExecutor&& other)
      : m_use_event_system(other.m_use_event_system),
        m_default_executor(other.m_default_executor),
        m_executors(std::move(other.m_executors)) {}

  Ret Execute(const ExecutedType& d, Args&&... args) {
    std::thread::id thread_id = d.m_thread_id;
    InvokeType invoke_type = d.m_invoke_type;
    if (invoke_type == InvokeType::Auto) {
      if (thread_id == std::this_thread::get_id()) {
        invoke_type = InvokeType::Direct;
      } else {
        invoke_type = InvokeType::Queued;
      }
    }
    if (m_executors.contains(thread_id)) {
      return m_executors[thread_id].Execute(
          d.m_invocable, std::forward<Args>(args)..., invoke_type);
    }
    if (m_use_event_system) {
      if (auto th = Thread::FindRegistered(thread_id); th.has_value()) {
        m_executors[thread_id] =
            QueuedInternalExecutor(th.value()->CallbackQueue());
        return m_executors[thread_id].Execute(
            d.m_invocable, std::forward<Args>(args)..., invoke_type);
      }
    }
    return m_default_executor.Execute(
        d.m_invocable, std::forward<Args>(args)..., d.m_invoke_type);
  }

 private:
  bool m_use_event_system;
  DefaultInternalExecutor m_default_executor;
  std::unordered_map<std::thread::id, QueuedInternalExecutor> m_executors;
};

}  // namespace delegates

}  // namespace eh
