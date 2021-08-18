#include "chrono_ex.hpp"
#include <iostream>
#include <cassert>
#include "algorithm.hpp"

using namespace rola;

int main_chronoex()
{
	rola::chrono::days year2(333);
	std::cout << year2 << std::endl;

	std::chrono::milliseconds d(42);
	std::cout << d << std::endl;

	rola::chrono::YMDHMS ymdhms = rola::split_ymdhms(std::chrono::system_clock::now());
	std::cout << ymdhms;

	constexpr int epcho = days_from_civil(1970, 1, 1);
	assert(epcho == 0);

	constexpr int epcho1 = days_from_civil(1970, 1, 2);
	assert(epcho1 == 1);

	constexpr int epchop1 = days_from_civil(1969, 12, 31);
	assert(epchop1 == -1);

	auto now_ms = get_ms(std::chrono::system_clock::now());
	std::cout << "\nnow time(ms): " << now_ms;
	auto hms = split_hms(now_ms);
	std::cout << "\n" << hms;

	int year = 1980;
	int month = 9;
	int day = 28;
	int hour = 14;
	int minute = 39;
	int second = 2;
	int ms = 392;

	auto t3 = from_ymdhms(year, month, day, hour, minute, second, ms);
	auto st3 = split_ymdhms(t3);

	assert(st3.year == year);
	assert(st3.month == month);
	assert(st3.day == day);
	assert(st3.hour == hour);
	assert(st3.minute = minute);
	assert(st3.second == second);
	assert(st3.ms == ms);

	std::cout << "\n" << to_secs_since_epoch(std::chrono::system_clock::now());
	std::cout << "\n" << to_msecs_since_epoch(std::chrono::system_clock::now());

	std::cout << "chrono_ex main successful\n";

	return 0;
}