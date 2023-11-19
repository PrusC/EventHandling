#pragma once

#include <future>
#include <memory>
#include <thread>

#include "thread.hpp"
#include "utils/invocable_element.hpp"
#include "utils/wrapped_call.hpp"
#include "invoke_type.h"

namespace eh {

namespace delegates {

template <typename Ret, typename... Args>
class InternalExecutor {
 public:
  virtual Ret Execute(const utils::InvocableBasePtr<Ret, Args...>& invocable,
                      Args&&... args, InvokeType invoke_type) = 0;
};

template <typename Ret, typename... Args>
using ExecutorPtr = std::shared_ptr<InternalExecutor<Ret, Args...>>;

template <typename Ret, typename... Args>
class DefaultInternalExecutor : public InternalExecutor<Ret, Args...> {
 public:
  Ret Execute(const utils::InvocableBasePtr<Ret, Args...>& invocable,
              Args&&... args, InvokeType /*invoke_type*/) override {
    return invocable->invoke(std::forward<Args>(args)...);
  }
};

template <typename Ret, typename... Args>
class QueuedInternalExecutor : public InternalExecutor<Ret, Args...> {
  using WrapInvoke = utils::WrappedCallImpl<Ret, Args...>;

 public:
  QueuedInternalExecutor() = default;
  explicit QueuedInternalExecutor(const utils::ICallbackQueuePtr& callback_queue)
      : m_callback_queue(callback_queue) {}
  QueuedInternalExecutor(const QueuedInternalExecutor& other)
      : m_callback_queue(other.m_callback_queue) {}

  Ret Execute(const utils::InvocableBasePtr<Ret, Args...>& invocable,
              Args&&... args, InvokeType invoke_type) override {
    if (invoke_type == InvokeType::Auto) {
      invoke_type = InvokeType::Direct;
    }
    if (invoke_type == InvokeType::Queued ||
        invoke_type == InvokeType::BlockQueued) {
      if (!m_callback_queue.expired()) {
        m_callback_queue_locked = m_callback_queue.lock();
      }
      if (m_callback_queue_locked == nullptr) {
        invoke_type = InvokeType::Direct;
      }
    }
    switch (invoke_type) {
      case InvokeType::Direct:
        return ExecuteDirect(invocable, std::forward<Args>(args)...);

      case InvokeType::Async:
        return ExecuteAsync(invocable, std::forward<Args>(args)...);

      case InvokeType::Queued:
        if constexpr (std::is_same_v<Ret, void>) {
          ExecuteQueued(invocable, std::forward<Args>(args)...);
          return;
        } else {
          [[fallthrough]];
        }

      case InvokeType::BlockQueued:
        return ExecuteBlockQueued(invocable, std::forward<Args>(args)...);
    }
    return invocable->invoke(std::forward<Args>(args)...);
  }

 private:
  utils::ICallbackQueuePtr m_callback_queue_locked;
  utils::ICallbackQueueWPtr m_callback_queue;

  Ret ExecuteDirect(const utils::InvocableBasePtr<Ret, Args...>& invocable,
                    Args&&... args) {
    return invocable->invoke(std::forward<Args>(args)...);
  }

  Ret ExecuteAsync(const utils::InvocableBasePtr<Ret, Args...>& invocable,
                   Args&&... args) {
    WrapInvoke call(invocable, std::forward<Args>(args)...);
    auto future = call.GetFuture();
    std::jthread th(&WrapInvoke::Perform, std::ref(call));
    return future.get();
  }

  void ExecuteQueued(const utils::InvocableBasePtr<Ret, Args...>& invocable,
                     Args&&... args) {
    std::shared_ptr<WrapInvoke> call =
        std::make_shared<WrapInvoke>(invocable, std::forward<Args>(args)...);
    m_callback_queue_locked->addCallback(call);
    m_callback_queue_locked.reset();
  }

  Ret ExecuteBlockQueued(const utils::InvocableBasePtr<Ret, Args...>& invocable,
                         Args&&... args) {
    std::shared_ptr<WrapInvoke> call =
        std::make_shared<WrapInvoke>(invocable, std::forward<Args>(args)...);
    m_callback_queue_locked->addCallback(call);
    auto future = call->GetFuture();
    m_callback_queue_locked.reset();
    return future.get();
  }
};

template <typename Ret, typename... Args>
class ThreadedInternalExecutor : public InternalExecutor<Ret, Args...> {
 public:
  ThreadedInternalExecutor()
      : m_thread(Thread::Create()), m_executor(m_thread->CallbackQueue()) {}
  ThreadedInternalExecutor(const ThreadPtr& th)
      : m_thread(th), m_executor(th->CallbackQueue()) {}
  ThreadedInternalExecutor(const ThreadedInternalExecutor& other)
      : m_thread(other.m_thread), m_executor(other.m_executor) {}

  Ret Execute(const utils::InvocableBasePtr<Ret, Args...>& invocable,
              Args&&... args, InvokeType invoke_type) override {
    return m_executor.Execute(invocable, std::forward<Args>(args)...,
                              invoke_type);
  }

 private:
  ThreadPtr m_thread;
  QueuedInternalExecutor<Ret, Args...> m_executor;
};

}  // namespace delegates

}  // namespace eh
