#include "easy_signal.hpp"

#include <iostream>
#include <numeric>

std::string func(double)
{
	std::cout << "std::string func(double)\n";
	return "func";
}

auto l = [](double) -> std::string
{
	std::cout << "lambda: double -> std::string\n";
	return "lambda";
};

class Bar
{
public:
	std::string operator()(double) const
	{
		std::cout << "Bar::operator()(double)\n";
		return "operator()";
	}

	std::string foo(double) const
	{
		std::cout << "Bar::foo -> double:std::string\n";
		return "Bar::foo()";
	}
};

int func2()
{
	return 3;
}

class Bar2
{
public:
	int operator()() const
	{
		return 10;
	}

	int Foo2() const
	{
		return 21;
	}
};

int sum_agg(const std::vector<int>& v)
{
	return std::accumulate(v.cbegin(), v.cend(), 0);
}

// basic test: connect, disconnect, reconnect
int main()
{
	rola::Easy_signal<std::string(double)> sig;
	auto conn1 = sig.connect(func);
	auto conn2 = sig.connect(l);
	auto conn3 = sig.connect(Bar());
	Bar b;
	auto conn4 = sig.connect(&Bar::foo, b);
	sig(3.22); // emit signal

	std::cout << "press any key to disconnect conn1...\n";
	std::cin.get();
	conn1.disconnect();
	sig(3.01);

	std::cout << "press any key to disconnect conn2...\n";
	std::cin.get();
	conn2.disconnect();
	sig(2.93);

	std::cout << "press any key to disconnect conn3...\n";
	std::cin.get();
	conn3.disconnect();
	sig(3.22);

	std::cout << "press any key to disconnect conn4...\n";
	std::cin.get();
	conn4.disconnect();
	sig(3.23);

	std::cout << "press any key to reconnect conn2...\n";
	std::cin.get();
	conn2.reconnect();
	sig(0);

	std::cout << "press any key to reconnect conn4...\n";
	std::cin.get();
	conn4.reconnect();
	sig(0.23);

	std::cout << "press any key to reconnect conn3...\n";
	std::cin.get();
	conn3.reconnect();
	sig(0.99);

	std::cout << "press any key to reconnect conn1...\n";
	std::cin.get();
	conn1.reconnect();
	sig(3.22);

	return 0;
}