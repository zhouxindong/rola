#include <future>
#include <thread>

void produce_value(std::promise<int>& p)
{
	p.set_value(42);
}

void consume_value(std::future<int>& f)
{
	auto value = f.get();
}

void promise_demo()
{
	std::promise<int> p;
	std::thread t1(produce_value, std::ref(p));

	std::future<int> f = p.get_future();
	std::thread t2(consume_value, std::ref(f));

	t1.join();
	t2.join();
}