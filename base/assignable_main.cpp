#include "assignable.hpp"
#include <iostream>

template <typename Function>
class Foo
{
public:
	Function fun;

	template <typename Callable>
	explicit Foo(Callable&& c)
		: fun(std::forward<Callable>(c))
	{}

	Foo() = default;
};

auto lambda1 = []() {std::cout << "lambda1\n"; };

template <typename Function>
class Foo2
{
public:
	rola::base::Assignable<Function> fun;

	template <typename Callable>
	explicit Foo2(Callable&& c)
		: fun(std::forward<Callable>(c))
	{}

	Foo2() = default;
};

int main()
{
	Foo<decltype(lambda1)> f1(lambda1);
	Foo<decltype(lambda1)> f2 = f1;
	//f2 = f1; // operator= of lambda is delete!

	Foo2<decltype(lambda1)> foo1(lambda1);
	Foo2<decltype(lambda1)> foo2 = foo1;
	foo2 = foo1; // operator= success
	foo2.fun();

	std::cout << "assignable_main successful" << std::endl;
	return 0;
}