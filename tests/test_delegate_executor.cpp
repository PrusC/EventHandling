#include <gtest/gtest.h>

#include <EventHandling/delegate_executor.hpp>
#include <EventHandling/delegate.hpp>
#include <EventHandling/thread.hpp>
#include <EventHandling/event_system.h>

#include "util_functions.h"

using namespace eh::delegates;

std::thread::id GetCurrentThreadId() { return std::this_thread::get_id(); }

TEST(Test_delegate_executor, test_simple) {
  Delegate<int, int, int> d = delegate<int, int, int>(Sum);
  DelegateExecutor<int, int, int> executor;

  ASSERT_EQ(6, executor.Execute(d, 1, 5));
}

TEST(Test_delegate_executore, test_with_event_system) {
  eh::EventSystem::Init();
  Delegate<std::thread::id> d1 = delegate<std::thread::id>(GetCurrentThreadId);
  Delegate<std::thread::id> d2 = delegate<std::thread::id>(GetCurrentThreadId);

  DelegateExecutor<std::thread::id> executor(true);

  auto th1 = eh::Thread::CreateRegistered();
  th1->Start();
  auto th2 = eh::EventSystem::MainThread();
  th2->Start();
  ASSERT_NE(th1->ThreadId(), th2->ThreadId());

  d1.SetThreadId(th1->ThreadId());
  d2.SetThreadId(th2->ThreadId());

  ASSERT_EQ(executor.Execute(d1), th1->ThreadId());
  ASSERT_EQ(executor.Execute(d2), th2->ThreadId());

  d1.SetInvokeType(InvokeType::Direct);
  ASSERT_NE(executor.Execute(d1), th1->ThreadId());
  ASSERT_EQ(executor.Execute(d1), std::this_thread::get_id());

  eh::EventSystem::Release();
}