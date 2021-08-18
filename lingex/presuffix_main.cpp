#include <iostream>
#include "presuffix.hpp"

void prefix()
{
	std::cout << "prefix() called" << std::endl;
}

void suffix()
{
	std::cout << "suffix() called" << std::endl;
}

class X
{
public:
	X()
	{
		std::cout << "X::X()" << std::endl;
	}

	int f() const
	{
		std::cout << "X::f()" << std::endl;
		return 1;
	}

	void g() const
	{
		std::cout << "X::g()" << std::endl;
	}
};

class Y
{
public:
	Y()
	{
		std::cout << "Y::Y()" << std::endl;
	}

	void h() const
	{
		std::cout << "Y::h()" << std::endl;
	}
};

int main_presuffix()
{
	rola::Presuffix<X> xx(new X);
	rola::Presuffix<Y> yy(new Y);
	xx.set_prefix(prefix);
	yy.set_suffix(suffix);
	if (xx->f()) std::cout << "done\n";
	xx->g();
	yy->h();

	std::cout << "presuffix_main successful" << std::endl;
	return 0;
}