#include "any_value.hpp"
#include <iostream>
#include <cassert>

class Foo
{};

int main()
{
	rola::Any_value a;
	assert(!a);

	rola::Any_value i1 = 99;
	assert(i1.can_as<int>());
	assert(!i1.can_as<double>());
	assert(i1.as<int>() == 99);
	assert(i1.as<int>() != 0);

	rola::Any_value i2 = i1;
	assert(i2.can_as<int>());
	assert(!i2.can_as<double>());
	assert(i2.as<int>() == 99);
	assert(i2.as<int>() != 0);

	rola::Any_value i3 = std::move(i2);
	assert(!i2);
	assert(i3.can_as<int>());
	assert(!i3.can_as<double>());
	assert(i3.as<int>() == 99);
	assert(i3.as<int>() != 0);

	rola::Any_value b = std::string("abcd");
	assert(b.as<std::string>() == std::string("abcd"));
	rola::Any_value c = 39.89;
	assert(c.as<double>() == 39.89);
	rola::Any_value d = Foo();
	assert(d.can_as<Foo>());

	a = b;
	assert(a.can_as<std::string>());
	assert(a.as<std::string>() == std::string("abcd"));
	a = std::move(d);
	assert(!a.can_as<std::string>());
	assert(a.can_as<Foo>());
	a = std::string("hello");
	assert(a.can_as<std::string>());
	assert(a.as<std::string>() == std::string("hello"));

	std::cout << "\nany_value_main.cpp successful\n";
	return 0;
}