#include "thread.hpp"
#include <cassert>

int foo(double d)
{
	return 0;
}

void bar()
{
}

int main()
{
	auto f = std::async(std::launch::async, foo, 3.23);
	assert(!rola::stlex::is_defered(f));

	auto f2 = std::async(std::launch::deferred, foo, 98.2);
	assert(rola::stlex::is_defered(f2));

	auto f3 = std::async(std::launch::async, bar);
	assert(!rola::stlex::is_defered(f3));

	auto f4 = std::async(std::launch::deferred, bar);
	assert(rola::stlex::is_defered(f4));
	return 0;
}