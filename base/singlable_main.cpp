#include "singlable.hpp"

#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;
using namespace rola::base;

class Foo
{
public:
	Foo(int ii, double dd) : i(ii), d(dd) {
		//pi = &i;
	}
	int i;
	double d;
	//int* pi;

	//bool operator==(Foo const& rhs)
	//{
	//	return i == rhs.i && d == rhs.d && pi == rhs.pi;
	//}
};

void thread_proc(Foo* f) // use reference assert is failed!
{
	for (int i = 0; i < 10000; ++i)
	{
		Foo* ff = Singlable<Foo>::instance_p(1, 0);
		assert(ff == f);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}
int main_sm()
{
	Foo* f = Singlable<Foo>::instance_p(3, 2.33);

	thread t(thread_proc, f);
	for (int i = 0; i < 1000; ++i)
	{
		Foo* f2 = Singlable<Foo>::instance_p(3, 99.23);
		assert(f2 == f);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	t.join();

	std::cout << "base::singlable test successful\n";

	return 0;
}