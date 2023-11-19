#include "thread.hpp"
#include "event_system.h"
#include "utils/callback_queue.hpp"
#include <iostream>

namespace eh {

//struct Thread::ThreadData {
//  ThreadData();
//
//  std::shared_ptr<utils::ICallbackQueue> m_queue;
//  bool m_is_running;
//  mutable std::recursive_mutex m_mutex;
//};

Thread::ThreadData::ThreadData()
    : m_queue(std::make_shared<utils::CallBackQueue>()), m_is_running(false) {}

Thread::Thread() : m_data(std::make_unique<Thread::ThreadData>()) {}

ThreadPtr Thread::Create() {
  return std::shared_ptr<Thread>(new Thread());
}

ThreadPtr Thread::CreateRegistered() {
  auto th = Thread::Create();
  th->Start();
  EventSystem::Instance().RegisterThread(th);
  return th;
}

 std::optional<ThreadPtr> Thread::FindRegistered(std::thread::id id) {
  if (EventSystem::Instance().ContainsRegistererdThread(id)) {
    return EventSystem::Instance().GetRegisteredThread(id);
  }
  return std::nullopt;
 }

Thread::~Thread() {
  std::lock_guard<std::recursive_mutex> g(m_data->m_mutex);
  if (IsRunning()) {
    Stop();
  }
  //m_data->m_is_running = false;
  //if (m_thread != nullptr && m_thread->joinable()) {
  //  m_thread->join();
  //}
}

void Thread::Start() {
  std::lock_guard<std::recursive_mutex> g(m_data->m_mutex);
  if (!IsRunning()) {
    m_data->m_is_running = true;
    m_thread = std::make_unique<std::thread>(&Thread::ThreadFunc, this);
  }
}

void Thread::Stop() {
  std::lock_guard<std::recursive_mutex> g(m_data->m_mutex);
  if (IsRunning()) {
    m_data->m_is_running = false;
  }
  if (m_thread != nullptr && m_thread->joinable()) {
    m_thread->join();
  }
  m_thread.reset();
}

bool Thread::IsRunning() const {
  std::lock_guard<std::recursive_mutex> g(m_data->m_mutex);
  return m_data->m_is_running && m_thread != nullptr;
}

std::thread::id Thread::ThreadId() const {
  std::lock_guard<std::recursive_mutex> g(m_data->m_mutex);
  if (m_thread != nullptr) {
    return m_thread->get_id();
  }
  return std::thread::id();
}

const utils::ICallbackQueuePtr& Thread::CallbackQueue() const {
  std::lock_guard<std::recursive_mutex> g(m_data->m_mutex);
  return m_data->m_queue;
}

void Thread::ThreadFunc() {
  while (m_data->m_is_running) {
    ProcessQueue();
  }
}

void Thread::ProcessQueue() {
  if (!m_data->m_queue->empty()) {
    auto current = m_data->m_queue->Get();
    if (current != nullptr) {
      current->Perform();
    }
  }
}

}  // namespace eh
