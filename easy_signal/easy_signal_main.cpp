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
		std::cout << "Bar2::Foo2\n";
		return 21;
	}
};

int sum_agg(const std::vector<int>& v)
{
	return std::accumulate(v.cbegin(), v.cend(), 0);
}

class BBar2 : public Bar2
{
};

class Bar3
{
public:
	Bar3(rola::Easy_signal<int()>* sig)
	{
		sig->connect(&Bar3::Foo3, *this);
	}

private:
	int Foo3()
	{
		return 999;
	}
};

int main_signal()
{
	rola::Easy_signal<int()> sig;

	BBar2 b2;
	sig.connect(&BBar2::Foo2, *(static_cast<Bar2*>(&b2)));
	sig.emit_();

	return 0;
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
	auto conn11 = conn1;
	conn11.disconnect();
	sig(3.01);

	std::cout << "press any key to disconnect conn2...\n";
	std::cin.get();
	//conn2.disconnect();
	auto conn22 = conn2;
	conn22.disconnect();
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
	conn22.reconnect();
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
	conn11.reconnect();
	sig(3.22);

	std::cout << "press any key to signal connect to signal...\n";
	std::cin.get();
	rola::Easy_signal<std::string(double)> sigg;
	sigg.connect(func);
	sigg.connect(l);
	sigg.connect(Bar());

	rola::Easy_signal<std::string(double)> sigg2;
	Bar b2;
	sigg2.connect(&Bar::foo, b2);

	sigg.connect(sigg2);
	sigg(9.23);

	std::cout << "press any key to handle the connect results";
	std::cin.get();
	rola::Easy_signal<int()> sig5;
	sig5.connect(func2);
	sig5.connect([]() {return 33; });
	sig5.connect(Bar2());
	Bar2 bb2;
	sig5.connect(&Bar2::Foo2, bb2);
	Bar3 bar3(&sig5);
	std::cout << sig5.emit_handle_result(sum_agg) << "\n";

	std::cout << "\n--------easy_signal basic test successful---------\n";

	return 0;
}