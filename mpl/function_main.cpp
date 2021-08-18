#include "function.hpp"
#include <iostream>
#include <string>
#include <cassert>

using namespace std;

double foo(int i, string s)
{
	cout << "double foo(int, string)\n";
	return 0.99;
}

class X {
public:
	double operator()(int i, string s) const
	{
		cout << "double X::operator(int, string)\n";
		return 32.3;
	}

	static double bar(int i, string s)
	{
		cout << "static double X::bar(int, string)\n";
		return 92.2;
	}
};

auto lamb = [](int i, string s) -> double {
	cout << "double lambda(int, string)\n";
	return 98.323;
};

int main_fm()
{
	rola::Function<double(int, string)> f; // default ctor
	assert(!f);

	rola::Function<double(int, string)> f2 = foo; // ctor(T&&)
	f2(3, "hello");

	rola::Function<double(int, string)> f3 =
		static_cast<rola::Function<double(int, string)> const>(f2); // ctor(const&)
	f3(3, "hello");

	rola::Function<double(int, string)> f4 = f2; // ctor(&)
	f4(3, "hello");

	rola::Function<double(int, string)> f5 = std::move(f2); // ctor(&&);
	f5(3, "hello");

	rola::Function<double(int, string)> const f6 = foo;
	f6(3, "hello");

	f = f6; // =(const&)
	f(3, "hello");

	f = f5; // =(&)
	f(3, "hello");

	f = std::move(f3); // =(&&)
	f(3, "hello");

	f = foo; // =(T&&)
	f(3, "hello");

	f = X(); // =(T&&)
	f(3, "hello");

	rola::Function<double(int, string)> f7 = lamb;
	f7(3, "helloo");

	f = lamb;
	f(3, "hello");

	f = &X::bar;
	f(3, "hello");

	std::cout << "\nfunction_main.cpp successful\n";
	return 0;
}