#include <iostream>
#include "bit_vector.hpp"

int main_bitvector()
{
	rola::bit_vector bv;
	bv.add(true).add(true).add(false);
	bv.add(false);
	bv.add(true);
	if (bv.any())
		std::cout << "has some 1s\n";
	if (bv.all())
		std::cout << "has only 1s\n";
	if (bv.none())
		std::cout << "has no 1s\n";

	std::cout << "has " << bv.count() << " 1s\n";

	bv.set(2, true);
	bv.set();

	bv.reset(0);
	bv.reset();

	bv.flip();

	std::cout << "bit vector successful\n";
	return 0;
}