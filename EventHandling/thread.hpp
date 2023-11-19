#pragma once

#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <optional>

#include "utils\callback_queue_base.hpp"

namespace eh {

class Thread : public std::enable_shared_from_this<Thread> {
 public:
  using Ptr = std::shared_ptr<Thread>;
  using WPtr = std::shared_ptr<Thread>;

  static Ptr Create();
  static Ptr CreateRegistered();
  static std::optional<Ptr> FindRegistered(std::thread::id id);

  ~Thread();

  void Start();
  void Stop();
  bool IsRunning() const;
  std::thread::id ThreadId() const;
  const utils::ICallbackQueuePtr& CallbackQueue() const;

 private:
  Thread();
  Thread(const Thread& other) = delete;
  Thread(Thread&& other) = delete;
  void ThreadFunc();
  void ProcessQueue();

  //struct ThreadData;
  struct ThreadData {
    ThreadData();

    std::shared_ptr<utils::ICallbackQueue> m_queue;
    std::atomic_bool m_is_running;
    mutable std::recursive_mutex m_mutex;
  };

  std::unique_ptr<std::thread> m_thread;
  std::unique_ptr<ThreadData> m_data;
};

using ThreadPtr = Thread::Ptr;
using ThreadWPtr = Thread::WPtr;

}  // namespace eh