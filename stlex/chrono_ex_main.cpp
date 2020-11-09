#include "chrono_ex.hpp"
#include <iostream>

using namespace rola;

int main()
{
	rola::chrono::days year2(333);
	std::cout << year2 << std::endl;

	std::chrono::milliseconds d(42);
	std::cout << d << std::endl;

	rola::chrono::YMDHMS ymdhms = rola::split_ymdhms(std::chrono::system_clock::now());
	std::cout << ymdhms;

	return 0;
}