#include "vtable.hpp"
#include <vector>

int main()
{
	animal a = cat{};
	a.speak();
	a = dog{};
	a.speak();

	animal b = a;
	b.speak();

	

	std::vector<animal> animals{ cat{}, dog{} };
	for (auto&& a : animals)
	{
		a.speak();
	}

	std::cout << "vtable_main.cpp successful\n";
	return 0;
}