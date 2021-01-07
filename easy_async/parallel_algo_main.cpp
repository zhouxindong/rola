#include "easy_async.hpp"
#include <iostream>

static int buf[10];

static int* get_numbers()
{
	return buf;
}

static size_t get_num_numbers()
{
	return 10;
}

int main()
{
	// parallel_invoke
	async::parallel_invoke(
		[] {
			std::cout << "This is executed in parallel..." << std::endl;
		},
		[] {
			std::cout << "with this" << std::endl;
		}
		);

	// parallel_for
	async::parallel_for({ 0,1,2,3,4 },
		[](int x) {
			std::cout << x;
		}
	);
	std::cout << std::endl;

	// parallel_reduce
	int r = async::parallel_reduce({ 1,2,3,4 }, 0,
		[](int x, int y)
		{
			return x + y;
		}
	);
	std::cout << "The sum of {1,2,3,4} is " << r << std::endl;

	// parallel_map_reduce
	int r2 = async::parallel_map_reduce({ 1,2,3,4 }, 0,
		[](int x)
		{
			return x * 2;
		},
		[](int x, int y)
		{
			return x + y;
		}
		);
	std::cout << "The sum of {1,2,3,4} with each element doubled is "
		<< r2 << std::endl;

	// range
	async::parallel_for(async::irange(0, 5),
		[](int x)
		{
			std::cout << x;
		}
	);
	std::cout << std::endl;

	int* numbers = get_numbers();
	size_t num_numbers = get_num_numbers();
	async::parallel_for(async::make_range(numbers, numbers + num_numbers),
		[](int x)
		{
			std::cout << x;
		});
	std::cout << std::endl;

	async::parallel_for(async::static_partitioner(async::irange(0, 1024), 8),
		[](int x)
		{
			std::cout << x;
		}
	);
	std::cout << std::endl;

	std::cout << "parallel_algo_main successful" << std::endl;
	return 0;
}