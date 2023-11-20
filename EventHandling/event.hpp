#pragma once

#include "event_handler.hpp"

namespace eh {

namespace events {

enum class TriggerType { Synchronous, Asynchronous };

template <typename... Args>
class IEvent {
 public:
  virtual ~IEvent() {}

  virtual bool HasHandlers() const = 0;
  virtual void AddHandler(const EventHandler<Args...>& handler) = 0;
  virtual void RemoveHandler(const EventHandler<Args...>& handler) = 0;
  virtual void SyncTrigger(Args&&... args) = 0;
  virtual void AsyncTrigger(Args&&... args) = 0;
  virtual TriggerType GetTriggerType() const = 0;
  virtual void SetTriggerType(TriggerType trigger_type) = 0;

  void Trigger(Args&&... args) const {
    switch (TriggerType()) {
      case TriggerType::Synchronous:
        return SyncTrigger(std::forward<Args>(args)...);
      case TriggerType::Asynchronous:
        return AsyncTrigger(std::forward<Args>(args)...);
      default:
        return SyncTrigger(std::forward<Args>(args)...);
    }
  }

 protected:
};

template <typename... Args>
class Event : public IEvent<Args...> {
 public:
  Event() : m_trigger_type(TriggerType::Synchronous), m_handlers() {}

  bool HasHandlers() const override { return !m_handlers.IsEmpty(); }

  void AddHandler(const EventHandler<Args...>& handler) override {
    m_handlers += handler;
  }
  void RemoveHandler(const EventHandler<Args...>& handler) override {
    m_handlers -= handler;
  }

  void SyncTrigger(Args&&... args) override {
    m_handlers.SetUseExecutor(false);
    m_handlers.Invoke(std::forward<Args>(args)...);
  }
  void AsyncTrigger(Args&&... args) override {
    m_handlers.SetUseExecutor(true);
    m_handlers.Invoke(std::forward<Args>(args)...);
  }

  TriggerType GetTriggerType() const override { return m_trigger_type; }
  void SetTriggerType(TriggerType trigger_type) override {
    m_trigger_type = trigger_type;
  }

 private:
  std::atomic<TriggerType> m_trigger_type;
  EventHandlersStorage<Args...> m_handlers;
};

}  // namespace events

}  // namespace eh
