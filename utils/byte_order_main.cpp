#include "byte_order.hpp"
#include <iostream>
#include <cassert>

int main_byteorder()
{
	//std::cout << "the platform byte order is : " << rola::Order_detector::get_order() << std::endl;

	uint64_t source = 1ull;
	uint64_t rsource = rola::reverse_byteorder(source);
	uint64_t rrsource = rola::reverse_byteorder(rsource);
	assert(source == rrsource);

	std::cout << source << std::endl;
	std::cout << rsource << std::endl;
	std::cout << rrsource << std::endl;

	uint8_t t8 = 2;
	uint8_t rt8 = rola::reverse_byteorder(t8);
	uint8_t rrt8 = rola::reverse_byteorder(rt8);
	assert(t8 == rrt8);

	std::cout << (int)rt8 << std::endl;
	std::cout << (int)rrt8 << std::endl;

	float f = 3.1415926f;
	float rf = rola::reverse_byteorder(f);
	float rrf = rola::reverse_byteorder(rf);
	assert(f == rrf);

	std::cout << rf << std::endl;
	std::cout << rrf << std::endl;

	double d = 99994999999.9999;
	double rd = rola::reverse_byteorder(d);
	double rrd = rola::reverse_byteorder(rd);
	assert(d == rrd);

	std::cout << rd << std::endl;
	std::cout << rrd << std::endl;

	return 0;
}