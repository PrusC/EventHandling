#include <gtest/gtest.h>

#include <EventHandling/utils/wrapped_call.hpp>

#include "util_functions.h"

using namespace eh::utils;

TEST(Test_wrapped_call, test_common) {
  auto invocable_function =
      InvocationElementFactory<int, int, int>::create(Sum);

  WrappedCallImpl<int, int, int> call(invocable_function, 2, 5);

  call.Perform();

  ASSERT_EQ(7, call.Retrieve());
}

TEST(Test_wrapped_call, test_error_while_perform) {
  auto invocable_function =
      InvocationElementFactory<int, int, int>::create(Sum);

  WrappedCallImpl<int, int, int> call(invocable_function, 2, 5);

  ASSERT_THROW({ auto resutl = call.Retrieve(); }, std::logic_error);

  invocable_function.reset();
  call.Perform();
  ASSERT_THROW({ auto resutl = call.Retrieve(); }, ArgumentPackApplyError);

}

TEST(Test_wrapped_call, test_use_future) {
  auto invocable_function =
      InvocationElementFactory<int, int, int>::create(Sum);

  WrappedCallImpl<int, int, int> call(invocable_function, 2, 5);
  auto future = call.GetFuture();
  call.Perform();
  ASSERT_EQ(7, future.get());
}
