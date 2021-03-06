#include "chrono_ex.hpp"
#include <iostream>
#include <cassert>
#include "algorithm.hpp"

using namespace rola;

int main_cem()
{
	rola::chrono::days year2(333);
	std::cout << year2 << std::endl;

	std::chrono::milliseconds d(42);
	std::cout << d << std::endl;

	rola::chrono::YMDHMS ymdhms = rola::split_ymdhms(std::chrono::system_clock::now());
	std::cout << ymdhms;

	int year = 1980;
	int month = 9;
	int day = 28;
	int hour = 14;
	int minute = 39;
	int second = 2;
	int ms = 289;

	return 0;
}