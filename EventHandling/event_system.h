#pragma once

#include <stdexcept>
#include <unordered_map>

#include "thread.hpp"

namespace eh {

class EventSystemError : std::runtime_error {
 public:
   using std::runtime_error::runtime_error;
};

class EventSystem {
 public:
  static EventSystem& Instance();
  static ThreadPtr MainThread();
  static void Init();
  static void Release();

  EventSystem() = default;
  ~EventSystem();

  void RegisterThread(const ThreadPtr& th);
  ThreadPtr GetRegisteredThread(std::thread::id id);
  bool ContainsRegistererdThread(std::thread::id id) const;

 private:
  mutable std::recursive_mutex m_mutex;
  std::unordered_map<std::thread::id, ThreadPtr> m_threads;
};

}  // namespace eh
