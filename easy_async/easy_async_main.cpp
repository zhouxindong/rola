#include "easy_async.hpp"
#include <iostream>

int main_easyasync()
{
	auto task1 = async::spawn([] {
		std::cout << "task1 executes asynchronously" << std::endl;
		});
	auto task2 = async::spawn([]()->int {
		std::cout << "task2 executes in parallel with task1" << std::endl;
		return 42;
		});
	auto task3 = task2.then([](int value)->int {
		std::cout << "task3 executes after task2, which returned " << value << std::endl;
		return value * 3;
		});
	auto task4 = async::when_all(task1, task3);
	auto task5 = task4.then(
		[](std::tuple<
			async::task<void>,
			async::task<int>> results) 
		{
			std::cout << "task5 executes after task1 and task3. task3 returned "
				<< std::get<1>(results).get() << std::endl;
		});
	task5.get();
	std::cout << "task5 has completed" << std::endl;

	async::parallel_invoke(
		[] {
			std::cout << "this is execute in parallel..." << std::endl;
		},
		[] {
			std::cout << "with this" << std::endl;
		}
		);

	async::parallel_for(async::irange(0, 5), [](int x) {
		std::cout << x << " ";
		});
	std::cout << std::endl;

	int r = async::parallel_reduce({ 1,2,3,4 }, 0, [](int x, int y) {
		return x + y;
		});
	std::cout << "The sum of {1,2,3,4} is " << r << std::endl;

	std::cout << "easy_async_main successful" << std::endl;
	return 0;
}