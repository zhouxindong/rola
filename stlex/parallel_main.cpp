#include <iostream>
#include <vector>
#include <iomanip>
#include <cassert>

#include "parallel.hpp"
#include "chrono_ex.hpp"

int main()
{
	std::vector<int> sizes
	{
		10000, 100000, 500000,
		1000000, 2000000, 5000000,
		10000000, 25000000, 50000000
	};

	std::cout
		<< std::right << std::setw(8) << std::setfill(' ') << "size"
		<< std::right << std::setw(8) << "s map"
		<< std::right << std::setw(8) << "p map"
		<< std::right << std::setw(8) << "s fold"
		<< std::right << std::setw(8) << "p fold"
		<< std::endl;

	for (auto const size : sizes)
	{
		std::vector<int> v(size);
		std::iota(std::begin(v), std::end(v), 1);

		auto v1 = v;
		auto s1 = 0LL;

		auto tsm = rola::perf_timer<>::duration([&]() {
			std::transform(std::begin(v1), std::end(v1), std::begin(v1),
				[](int const i) {return i + i; });
			});

		auto tsf = rola::perf_timer<>::duration([&]() {
			s1 = std::accumulate(std::begin(v1), std::end(v1), 0LL, std::plus<>());
			});

		auto v2 = v;
		auto s2 = 0LL;
		auto tpm = rola::perf_timer<>::duration([&]() {
			rola::parallel_map2(std::begin(v2), std::end(v2), [](int const i) {return i + i; });
			});

		auto tpf = rola::perf_timer<>::duration([&]() {
			s2 = rola::parallel_reduce2(std::begin(v2), std::end(v2), 0LL, std::plus<>());
			});

		assert(v1 == v2);
		assert(s1 == s2);

		std::cout
			<< std::right << std::setw(8) << std::setfill(' ') << size
			<< std::right << std::setw(8)
			<< std::chrono::duration<double, std::micro>(tsm).count()
			<< std::right << std::setw(8)
			<< std::chrono::duration<double, std::micro>(tpm).count()
			<< std::right << std::setw(8)
			<< std::chrono::duration<double, std::micro>(tsf).count()
			<< std::right << std::setw(8)
			<< std::chrono::duration<double, std::micro>(tpf).count()
			<< std::endl;
	}

	std::cout << "parallel main successful\n";
	return 0;
}