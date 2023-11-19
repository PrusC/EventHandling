#include "event_system.h"

#include <algorithm>
#include <atomic>

namespace eh {

std::atomic<std::shared_ptr<Thread>> main_thread;
std::unique_ptr<EventSystem> instance;


EventSystem& EventSystem::Instance() {
  //static std::unique_ptr<EventSystem> instance;
  if (instance == nullptr) {
    //instance = std::make_unique<EventSystem>();
    throw EventSystemError(
        "Event system was not inialized. Call EventSystem::Init befor");
  }
  return *instance;
}

ThreadPtr EventSystem::MainThread() {
  if (main_thread.load() == nullptr) {
    //main_thread.store(Thread::Create());
    //Instance().RegisterThread(main_thread.load());
    throw EventSystemError(
        "Event system was not inialized. Call EventSystem::Init befor");
  }
  return main_thread.load(); 
}

void EventSystem::Init() {
  main_thread.store(Thread::Create());
  main_thread.load()->Start();
  instance = std::make_unique<EventSystem>();
  instance->RegisterThread(main_thread.load());
}

void EventSystem::Release() { 
  main_thread.load()->Stop();
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
