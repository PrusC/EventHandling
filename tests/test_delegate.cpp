#include <gtest/gtest.h>

#include <EventHandling/delegate.hpp>
#include <EventHandling/multicast_delegate.hpp>

#include "util_functions.h"

using namespace eh::delegates;

TEST(Test_delegate, test_common) {
  auto delegate_function = delegate<int, int, int>(Sum);
  ASSERT_EQ(7, delegate_function(4, 3));

  SimpleClass s(5);
  auto delegate_method = delegate<int, int, int>(&s, &SimpleClass::Result);
  ASSERT_EQ(25, delegate_method(10, 2));


  auto delegate_static_method = delegate<int, int, int>(SimpleClass::Sum);
  ASSERT_EQ(7, delegate_static_method(4, 3));

  auto delegate_lambda =
      delegate<int, int, int>([](int a, int b) -> int { return a + b; });
  ASSERT_EQ(7, delegate_lambda(4, 3));

}

TEST(Test_delegate, test_error) {

  Delegate<void, int> d;
  ASSERT_TRUE(d.IsEmpty());

  ASSERT_THROW({ d.Invoke(2); }, std::runtime_error);
}

TEST(Test_delegate, test_multicast_delegate) {

  auto delegate_function = delegate<int, int, int>(Sum);

  SimpleClass s(5);
  auto delegate_method = delegate<int, int, int>(&s, &SimpleClass::Result);

  MulticastDelegate<int, int, int> md;

  md += delegate_function;
  ASSERT_EQ(4, md(2, 2));
  md += delegate_method;
  ASSERT_EQ(5, md(2, 2));
  auto md2 = delegate_function + delegate_method;
  ASSERT_EQ(md, md2);

  auto md3 = delegate_method + delegate_function;
  ASSERT_NE(md2, md3);
  ASSERT_NE(md, md3);

  md -= delegate_method;
  ASSERT_EQ(4, md(2, 2));

}