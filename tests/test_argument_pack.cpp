#include <gtest\gtest.h>

#include <EventHandling\utils\argument_pack.hpp>
#include <EventHandling\utils\invocable_element.hpp>

#include "util_functions.h"

TEST(Test_argument_pack, test_simple_variables) {
  eh::utils::ArgumentPack<int, int> args(2, 5);

  SimpleClass object(10);

  ASSERT_EQ(7, args.Apply(Sum));
  ASSERT_EQ(7, args.Apply(SimpleClass::Sum));

  auto invocable_method =
      eh::utils::InvocationElementFactory<int, int, int>::create(
          &object, &SimpleClass::Result);

  ASSERT_EQ(4, args.Apply(invocable_method));
  ASSERT_EQ(4, args.Apply(invocable_method->WeakCopy()));
}


TEST(Test_argument_pack, test_error) {
  eh::utils::ArgumentPack<int, int> args(2, 5);

  SimpleClass object(10);

  auto invocable_method =
      eh::utils::InvocationElementFactory<int, int, int>::create(
          &object, &SimpleClass::Result);

  auto invocable_method_weak = invocable_method->WeakCopy();
  invocable_method.reset();

  ASSERT_THROW({ auto result = args.Apply(invocable_method_weak); },
               eh::utils::ArgumentPackApplyError);
}