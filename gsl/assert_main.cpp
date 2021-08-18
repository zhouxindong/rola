#include <iostream>
#include "assert.hpp"
#include <cassert>

int f(int i)
{
	Expects(i > 0 && i < 10);
	return i;
}

int g(int i)
{
	i++;
	Ensures(i > 0 && i < 10);
	return i;
}

int main_assert()
{
	//std::set_terminate([] {
	//	std::cerr << "terminated: " << __FILE__ << "@" << __LINE__ ;
	//	std::abort();
	//	});

	f(80);

	assert(f(3) == 3);
	assert(g(2) == 3);
	std::cout << "assert_main.cpp successful\n";
	return 0;
}