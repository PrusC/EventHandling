#include <gtest/gtest.h>
#include "EventHandling/eventhandling.h"
#include <iostream>
#include <string>

int SimpleFunction() {
  return 12;
}

std::string FunctionWithInput(int n) {
  std::ostringstream os;
  os << n;
  return os.str();
}

class SomeClass {
public:
  int ConstFunct() const {
    return _a;
  }

  int ChangeFunct(int a) {
    _a += a;
    return _a;
  }

  static std::string StaticFunc(int a) {
    std::ostringstream os;
    os << a;
    return os.str();
  }

private:
  int _a = 5;
};


TEST(Tests, Test_holders) {
  using namespace events::holders;

  SomeClass a;
  MethodHolder<SomeClass, int> mh1(&a, &SomeClass::ConstFunct);
  MethodHolder<SomeClass, int, int> mh2(&a, &SomeClass::ChangeFunct);
  FunctionHolder<std::string, int>  mh3(&SomeClass::StaticFunc);
  FunctionHolder<int> fh(SimpleFunction);
  FunctionHolder<std::string, int> fh2(FunctionWithInput);

  ASSERT_EQ(mh1.invoke(), a.ConstFunct());
  ASSERT_EQ(mh2.invoke(5), a.ChangeFunct(5) + 5);
  ASSERT_EQ(mh3.invoke(5), SomeClass::StaticFunc(5));
  ASSERT_EQ(fh.invoke(), SimpleFunction());
  ASSERT_FALSE(mh3 == fh2);
}

TEST(Tests, Test_delegates) {
  using namespace events::delegates;

  SomeClass a;

  Delegate<int> d(SimpleFunction);
  Delegate<int, int> md = delegate<SomeClass, int, int>(&a, &SomeClass::ChangeFunct);
  Delegate<int, int> md2(&a, &SomeClass::ChangeFunct);

  ASSERT_EQ(d.invoke(), SimpleFunction());
  ASSERT_EQ(md.invoke(0), a.ChangeFunct(0));
  ASSERT_EQ(md2.invoke(0), a.ChangeFunct(0));
  ASSERT_TRUE(md == md2);

}