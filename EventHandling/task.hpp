#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <vector>

#include "utils/callback_queue_base.hpp"

namespace eh {

class Thread;

enum class TaskState { Init, Running, Halted, Finished, Cancelled };


class Task {
 public:
  Task();
  ~Task();

  bool IsRunning() const;
  bool IsFinished() const;
  bool IsCancelled() const;
  void Run();
  void Cancel();
  void DoWork();
  void RegisterCallbackQueue(const utils::ICallbackQueuePtr& callback);

 private:
  std::atomic<TaskState> m_state;
  std::recursive_mutex m_mutex;
  utils::ICallbackQueuePtr m_callback_queue;
  std::vector<utils::ICallbackQueuePtr> m_callback_queues;
  std::weak_ptr<Thread> m_thread;
};

}  // namespace eh
