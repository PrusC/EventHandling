#include <gtest/gtest.h>

#include <EventHandling/utils/wrapped_call.hpp>
#include <EventHandling/delegate_internal_executor.hpp>
#include <EventHandling/utils/callback_queue.hpp>
#include <EventHandling/thread.hpp>

#include "util_functions.h"

#include <thread>
#include <future>

using namespace eh::utils;
using namespace eh::delegates;

std::thread::id GetCurrentThreadId() { 
  return std::this_thread::get_id();
}

size_t counter = 0;

void increaseCounter(size_t value) { 
  counter += value; 
  std::cerr << "CurrentValue: " << counter << std::endl;
}

TEST(Test_internal_executor, test_default_executor) {
  auto invocable_function =
      InvocationElementFactory<int, int, int>::create(Sum);

  DefaultInternalExecutor<int, int, int> executor;

  ASSERT_EQ(7, executor.Execute(invocable_function, 2, 5, InvokeType::Direct));
}

TEST(Test_internal_executor, test_queued_executor_direct_async) {
  auto invocable_function =
      InvocationElementFactory<std::thread::id>::create(GetCurrentThreadId);

  QueuedInternalExecutor<std::thread::id> executor;

  ASSERT_EQ(std::this_thread::get_id(),
            executor.Execute(invocable_function, InvokeType::Direct));

  ASSERT_NE(std::this_thread::get_id(),
          executor.Execute(invocable_function, InvokeType::Async));
}

TEST(Test_internal_executor, test_queued_executor_queued) {
  auto invocable_function =
      InvocationElementFactory<void, size_t>::create(increaseCounter);

  auto callback_queue = std::make_shared<CallBackQueue>();
  QueuedInternalExecutor<void, size_t> executor(callback_queue);
  
  ASSERT_EQ(counter, 0);

  executor.Execute(invocable_function, 5, InvokeType::Queued);
  ASSERT_EQ(counter, 0);
  while (!callback_queue->empty()) {
    auto callback = callback_queue->Get();
    callback->Perform();
  }
  ASSERT_EQ(counter, 5);

  executor.Execute(invocable_function, 5, InvokeType::Queued);
  executor.Execute(invocable_function, 21, InvokeType::Queued);
  ASSERT_EQ(counter, 5);
  while (!callback_queue->empty()) {
    auto callback = callback_queue->Get();
    callback->Perform();
  }
  ASSERT_EQ(counter, 31);

}

TEST(Test_internal_executor, test_queued_executor_queued_threaded) {
  counter = 0;
  auto invocable_function =
      InvocationElementFactory<void, size_t>::create(increaseCounter);

  auto th = eh::Thread::Create();
  ASSERT_FALSE(th->IsRunning());
  th->Start();
  ASSERT_TRUE(th->IsRunning());
  QueuedInternalExecutor<void, size_t> executor(th->CallbackQueue());

  ASSERT_EQ(counter, 0);

  executor.Execute(invocable_function, 5, InvokeType::BlockQueued);
  ASSERT_EQ(counter, 5 );

  executor.Execute(invocable_function, 5, InvokeType::Queued);
  executor.Execute(invocable_function, 21, InvokeType::BlockQueued);
  ASSERT_EQ(counter, 31);

  th->Stop();
  ASSERT_FALSE(th->IsRunning());
}
