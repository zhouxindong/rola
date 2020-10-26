#include "type_list.hpp"
#include <type_traits>
#include <cassert>

#include <iostream>

class Base {};
class Derived1 : public Base {};
class Derived2 : public Base {};
class Derived11 : public Derived1 {};

using empty_typelist = rola::Type_list<>;
using integer_typelist = rola::Type_list<char, short, unsigned short,
	int, unsigned int, long long, unsigned long long>;
using float_typelist = rola::Type_list<float, double>;
using string_typelist = rola::Type_list<std::string>;

int main()
{
	// Length
	assert(0 == rola::TL::Length_v<empty_typelist>);
	assert(1 == rola::TL::Length_v<string_typelist>);
	assert(2 == rola::TL::Length_v<float_typelist>);
	assert(7 == rola::TL::Length_v<integer_typelist>);

	std::cout << "type_list test successful\n";

	return 0;
}