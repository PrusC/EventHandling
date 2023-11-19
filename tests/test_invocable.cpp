#include <gtest\gtest.h>

#include <EventHandling\utils\invocable_element.hpp>

#include "util_functions.h"

TEST(Test_invocable, test_simple_function) {
  auto invocable_function =
      eh::utils::InvocationElementFactory<int, int, int>::create(Sum);

  ASSERT_TRUE(invocable_function != nullptr);
  ASSERT_EQ(10, invocable_function->invoke(11, -1));

  auto copy_invocable_function = invocable_function->Copy();
  ASSERT_EQ(copy_invocable_function, invocable_function);
  ASSERT_EQ(copy_invocable_function.use_count(), 2);

  auto invocable_function2 =
      eh::utils::InvocationElementFactory<int, int, int>::create(Sum);
  ASSERT_NE(invocable_function, invocable_function2);
  ASSERT_EQ(*invocable_function, *invocable_function2);
}

TEST(Test_invocable, test_class_method) {
  SimpleClass object(2);

  auto invocable_method =
      eh::utils::InvocationElementFactory<int, int, int>::create(
          &object, &SimpleClass::Result);

  ASSERT_TRUE(invocable_method != nullptr);
  ASSERT_EQ(5, invocable_method->invoke(5, 2));

  auto copy_invocable_method = invocable_method->Copy();
  ASSERT_EQ(copy_invocable_method, invocable_method);
  ASSERT_EQ(copy_invocable_method.use_count(), 2);

  auto invocable_method2 =
      eh::utils::InvocationElementFactory<int, int, int>::create(
          &object, &SimpleClass::Result);
  ASSERT_NE(invocable_method, invocable_method2);
  ASSERT_EQ(*invocable_method, *invocable_method2);
}

TEST(Test_invocable, test_static_class_method) {
  auto invocable_method =
      eh::utils::InvocationElementFactory<int, int, int>::create(
          SimpleClass::Sum);

  ASSERT_TRUE(invocable_method != nullptr);
  ASSERT_EQ(10, invocable_method->invoke(11, -1));

  auto copy_invocable_method = invocable_method->Copy();
  ASSERT_EQ(copy_invocable_method, invocable_method);
  ASSERT_EQ(copy_invocable_method.use_count(), 2);

  auto invocable_method2 =
      eh::utils::InvocationElementFactory<int, int, int>::create(
          SimpleClass::Sum);
  ASSERT_NE(invocable_method, invocable_method2);
  ASSERT_EQ(*invocable_method, *invocable_method2);
}

TEST(Test_invocable, test_lambda) {
  auto sum_lambda = [](int a, int b) -> int { return a + b; };
  auto invocable =
      eh::utils::InvocationElementFactory<int, int, int>::create(sum_lambda);

  ASSERT_TRUE(invocable != nullptr);
  ASSERT_EQ(10, invocable->invoke(11, -1));

  auto copy_invocable = invocable->Copy();
  ASSERT_EQ(copy_invocable, invocable);
  ASSERT_EQ(copy_invocable.use_count(), 2);

  auto invocable2 =
      eh::utils::InvocationElementFactory<int, int, int>::create(sum_lambda);
  ASSERT_NE(invocable, invocable2);
  ASSERT_EQ(*invocable, *invocable2);
}

TEST(Test_invocable, test_lambda_with_move) {
  auto sum_lambda = [](int a, int b) -> int { return a + b; };
  auto invocable = eh::utils::InvocationElementFactory<int, int, int>::create(
      [](int a, int b) -> int { return a + b; });

  ASSERT_TRUE(invocable != nullptr);
  ASSERT_EQ(10, invocable->invoke(11, -1));

  auto copy_invocable = invocable->Copy();
  ASSERT_EQ(copy_invocable, invocable);
  ASSERT_EQ(copy_invocable.use_count(), 2);
  ASSERT_EQ(*copy_invocable, *invocable);

  auto invocable2 = eh::utils::InvocationElementFactory<int, int, int>::create(
      [](int a, int b) -> int { return a + b; });
  ASSERT_NE(invocable, invocable2);
  ASSERT_NE(*invocable, *invocable2);
}

TEST(Test_invocable, test_lambda_with_capture) {
  int N = 10;
  auto sum_lambda = [N](int a, int b) -> int { return N * (a + b); };
  auto invocable =
      eh::utils::InvocationElementFactory<int, int, int>::create(sum_lambda);

  ASSERT_TRUE(invocable != nullptr);
  ASSERT_EQ(100, invocable->invoke(11, -1));

  auto copy_invocable = invocable->Copy();
  ASSERT_EQ(copy_invocable, invocable);
  ASSERT_EQ(copy_invocable.use_count(), 2);
  ASSERT_NE(*copy_invocable, *invocable);

  auto invocable2 =
      eh::utils::InvocationElementFactory<int, int, int>::create(sum_lambda);
  ASSERT_NE(invocable, invocable2);
  ASSERT_NE(*invocable, *invocable2);
}

TEST(Test_invocable, test_different_types) {
  auto invocable_method =
      eh::utils::InvocationElementFactory<int, int, int>::create(
          &SimpleClass::Sum);

  auto invocable_function =
      eh::utils::InvocationElementFactory<int, int, int>::create(Sum);

  ASSERT_TRUE(invocable_method != nullptr);
  ASSERT_EQ(10, invocable_method->invoke(11, -1));

  ASSERT_TRUE(invocable_function != nullptr);
  ASSERT_EQ(10, invocable_function->invoke(11, -1));

  ASSERT_NE(invocable_function, invocable_method);
  ASSERT_NE(*invocable_function, *invocable_method);
}

TEST(Test_invocable, test_different_types2) {
  SimpleClass object(2);

  auto invocable_method =
      eh::utils::InvocationElementFactory<int, int, int>::create(
          &object, &SimpleClass::Result);

  auto invocable_static_method =
      eh::utils::InvocationElementFactory<int, int, int>::create(
          &SimpleClass::Sum);

  ASSERT_TRUE(invocable_method != nullptr);
  ASSERT_TRUE(invocable_static_method != nullptr);

  ASSERT_NE(invocable_method, invocable_static_method);
  ASSERT_NE(*invocable_method, *invocable_static_method);
}
