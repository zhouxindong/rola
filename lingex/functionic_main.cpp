#include <iostream>
#include <string>

#include "functionic.hpp"

class Foo
{
public:
	void operator()(const Foo&) const
	{
		std::cout << "Foo\n";
	}
};

class Bar
{
public:
	void operator()(const Bar&) const
	{
		std::cout << "Bar\n";
	}
};

class Jar
{
public:
	void operator()(const Jar&) const
	{
		std::cout << "Jar\n";
	}
};

int main()
{
	auto n = rola::compose(
		[](int const n) {return std::to_string(n); },
		[](int const n) {return n * n; },
		[](int const n) {return n + n; },
		[](int const n) {return std::abs(n); })(-3);

	std::cout << "\ncompose result: " << n << "\n";

	auto m = rola::overload_functor<Foo, Bar, Jar>{};
	m(Bar());
	m(Foo());
	m(Jar());

	std::cout << "functionic main successful\n";
	return 0;
}