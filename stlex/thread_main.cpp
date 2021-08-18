#include "thread.hpp"
#include <cassert>
#include <iostream>
#include "thread_safe_stack.hpp"
#include "thread_pool.hpp"

int foo(double d)
{
	return 0;
}

void bar()
{
}

int main_tm()
{
	auto f = std::async(std::launch::async, foo, 3.23);
	assert(!rola::stlex::is_defered(f));

	auto f2 = std::async(std::launch::deferred, foo, 98.2);
	assert(rola::stlex::is_defered(f2));

	auto f3 = std::async(std::launch::async, bar);
	assert(!rola::stlex::is_defered(f3));

	auto f4 = std::async(std::launch::deferred, bar);
	assert(rola::stlex::is_defered(f4));

	std::cout << "thread_main.cpp successful\n";
	return 0;
}