#include "task.hpp"

namespace eh {

Task::Task() : m_state(TaskState::Init) {}

Task::~Task() { Cancel(); }

bool Task::IsRunning() const { 
  return m_state == TaskState::Running; 
}

bool Task::IsFinished() const { return m_state == TaskState::Finished; }

bool Task::IsCancelled() const { return m_state == TaskState::Cancelled; }

void Task::Run() {
  m_state = TaskState::Running;
  while (!IsFinished() && !IsCancelled()) {
    DoWork();
  }
  m_state = TaskState::Finished;
}

void Task::Cancel() { m_state = TaskState::Cancelled; }

void Task::DoWork() {
  std::lock_guard<std::recursive_mutex> g(m_mutex);
  if (!m_callback_queue->empty()) {
    auto current = m_callback_queue->Get();
    if (current != nullptr) {
      current->Perform();
    }
  }
}

void eh::Task::RegisterCallbackQueue(const utils::ICallbackQueuePtr& callback) {
  std::lock_guard<std::recursive_mutex> g(m_mutex);
  m_callback_queue = callback;
}

}  // namespace eh