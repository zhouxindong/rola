#include <iostream>
#include "scpp_assert.hpp"

int main_scpp_assert()
{
	double stock_price = 100.0;
	SCPP_ASSERT(0. < stock_price && stock_price <= 1.e6,
		"Stock price " << stock_price << " is out of range");

	stock_price = -1.0;
	SCPP_ASSERT(0. < stock_price && stock_price <= 1.e6,
		"Stock price " << stock_price << " is out of range");

	std::cout << "scpp_assert_main successful\n";
	return 0;
}