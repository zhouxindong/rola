#include "guid.hpp"

#include <iostream>

int main2()
{
	std::cout << "len 24: " << rola::utils::guid_hex(24) << "\n";
	std::cout << "len 16: " << rola::utils::guid_hex(16) << "\n";
	std::cout << "len 8: " << rola::utils::guid_hex(8) << "\n";

	std::cout << "guid_hex test successful\n";
	return 0;
}