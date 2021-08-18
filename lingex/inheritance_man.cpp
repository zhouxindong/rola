#include "inheritance.hpp"
#include <iostream>
#include <vector>

struct ICalculator
{
	virtual long long compute(int input) const = 0;
	virtual ~ICalculator() {};
};

struct BigCalculator : ICalculator
{
	long long compute(int input) const override
	{
		std::cout << "BigCalculator::compute()" << std::endl;
		return input * 5;
	}
};

struct SmallCalculator : ICalculator
{
	long long compute(int input) const override
	{
		std::cout << "SmallCalculator::compute()" << std::endl;
		return input + 2;
	}
};

using Calculator = rola::Implementation<ICalculator>;

int main_inheritance()
{
	std::vector<Calculator> v;
	v.push_back(BigCalculator());
	v.push_back(SmallCalculator());

	std::cout << v[0]->compute(3) << std::endl;
	std::cout << v[1]->compute(3) << std::endl;

	std::cout << "inheritance_main successful" << std::endl;
	return 0;
}