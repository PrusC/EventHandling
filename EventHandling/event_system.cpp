#include "event_system.h"

#include <algorithm>

#ifndef __GNUC__
#include <atomic>
#endif  // !__GNUC__


namespace eh {

 #ifdef __GNUC__
std::mutex main_mutex;
std::shared_ptr<Thread> main_thread
#else
std::atomic<std::shared_ptr<Thread>> main_thread;
#endif  // __GNUC__

std::unique_ptr<EventSystem> instance;


EventSystem& EventSystem::Instance() {
  if (instance == nullptr) {
    //instance = std::make_unique<EventSystem>();
    throw EventSystemError(
        "Event system was not inialized. Call EventSystem::Init befor");
  }
  return *instance;
}

ThreadPtr EventSystem::MainThread() {
#ifdef __GNUC__
  std::lock_guard<std::mutex> g(main_mutex);
  if (main_thread == nullptr) {
    // main_thread.store(Thread::Create());
    // Instance().RegisterThread(main_thread.load());
    throw EventSystemError(
        "Event system was not inialized. Call EventSystem::Init befor");
  }
  return main_thread;
#else
  if (main_thread.load() == nullptr) {
    // main_thread.store(Thread::Create());
    // Instance().RegisterThread(main_thread.load());
    throw EventSystemError(
        "Event system was not inialized. Call EventSystem::Init befor");
  }
  return main_thread.load();
#endif  // __GNUC__
}

void EventSystem::Init() {
#ifdef __GNUC__
  std::lock_guard<std::mutex> g(main_mutex);
  main_thread = Thread::Create();
#else
  main_thread.store(Thread::Create());
  main_thread.load()->Start();
#endif  // __GNUC__

  instance = std::make_unique<EventSystem>();

#ifdef __GNUC__
  instance->RegisterThread(main_thread);
#else
  instance->RegisterThread(main_thread.load());
#endif  // __GNUC__

}

void EventSystem::Release() {
#ifdef __GNUC__ 
  {
    std::lock_guard<std::mutex> g(main_mutex);
    main_thread->Stop();
  }
#else
  main_thread.load()->Stop();
#endif  // __GNUC__

  instance.release();
}

EventSystem::~EventSystem() {
  std::lock_guard<std::recursive_mutex> g(m_mutex);
  for (auto& th : m_threads) {
    th.second->Stop();
    th.second = nullptr;
  }
}

void EventSystem::RegisterThread(const ThreadPtr& th) {
  if (th == nullptr) {
    return;
  }
  std::lock_guard<std::recursive_mutex> g(m_mutex);
  if (!m_threads.contains(th->ThreadId())) {
    m_threads[th->ThreadId()] = th;
  }
}

ThreadPtr EventSystem::GetRegisteredThread(std::thread::id id) {
  std::lock_guard<std::recursive_mutex> g(m_mutex);
  return m_threads.at(id);
}

bool EventSystem::ContainsRegistererdThread(std::thread::id id) const {
  std::lock_guard<std::recursive_mutex> g(m_mutex);
  return m_threads.contains(id);
}

}  // namespace eh
