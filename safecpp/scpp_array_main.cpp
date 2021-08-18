#include <algorithm>
#include <iostream>
#include "scpp_array.hpp"

int main_scpp_array()
{
	rola::array<int, 5> a{ 7,2,3,9,0 };
	std::cout << "Array before sort: " << a << std::endl;
	std::sort(a.begin(), a.end());
	std::cout << "Array after sort: " << a << std::endl;
	
	std::cout << "scpp_array successful\n";
	return 0;
}