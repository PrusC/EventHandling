#include <iostream>
#include <functional>
#include "EventHandling/eventhandling.h"

using namespace std;

int func() {
	return 12;
}

int func2(int n) {
	return n + 12;
}

class SomeClass {

public:
	int const_func(int a) const {
		return a * a;
	}

	int change_func(int a) {
		_a += a;
		return _a;
	}

	static void static_func(int a) {
		cout << a << endl;
	}

private:
	int _a = 0;
};

void test_holders() {

	using namespace events::holders;

	FunctionHolder<int> fh(func);
	cout << "Function Holder: " << fh() << endl;

	SomeClass a;
	MethodHolder<SomeClass, int, int> mh1(&a, &SomeClass::const_func);
	MethodHolder<SomeClass, int, int> mh2(&a, &SomeClass::change_func);
	FunctionHolder<void, int>  mh3(&SomeClass::static_func);

	cout << "Method Holder: " << mh1.invoke(5) << endl;
	cout << "Method Holder: " << mh2.invoke(5) << endl;
	cout << "Method Holder: "; mh3(313);
}


void test_delegates() {
	using namespace events::delegates;

	Delegate<int> d(func);
	cout << "FunctionDelegate: " << d() << endl;

	SomeClass a;
	Delegate<int, int> md = delegate<SomeClass, int, int>(&a, &SomeClass::change_func);
	cout << "MethodDelegate: " << md(4) << "   " << md(3) << endl;
	Delegate<int, int> md2(&a, &SomeClass::change_func);
	cout << "MethodDelegate2: " << md2(444) << endl;

	MulticastDelegate<int, int> multi;
	multi += md;
	multi += delegate<int, int>(func2);
	cout << "MulticastDelegate: " << multi(3) << endl;
	multi -= md;
	cout << "MulticastDelegate: " << multi(3) << endl;

}


int main() {

	test_holders();
	test_delegates();

	return 0;
}