#include "SFINAE.h"
#include <iostream>
#include <cassert>

#include "debug_template.h"
#include <vector>
#include <string>

struct A {};

std::string to_string(const A&)
{
	return "to_string(const A&)";
}

// Type B with a serialize method.
struct B
{
	std::string serialize() const
	{
		return "B::serialize()";
	}
};

// Type C with a "wrong" serialize member (not a method) and a to_string overload.
struct C
{
	std::string serialize;
};

std::string to_string(const C&)
{
	return "I am a C!";
}

class D
{
public:
	typedef size_t size_type;

	size_t size() const
	{
		return 99;
	}
};

template <typename T>
using containted_type_t = decltype(*std::begin(T()));

int main_sfinae()
{
	static_assert(!Has_serialize_98<std::string>::value);
	static_assert(!Has_serialize_98<A>::value);
	static_assert(Has_serialize_98<B>::value);
	static_assert(!Has_serialize_98<C>::value);

	A a;
	std::cout << serialize(a) << "\n";
	B b;
	std::cout << serialize(b) << "\n";

	

	static_assert(!Has_serialize_11_1<std::string>::value);
	static_assert(!Has_serialize_11_1<A>::value);
	static_assert(Has_serialize_11_1<B>::value);
	static_assert(!Has_serialize_11_1<C>::value);

	static_assert(!Has_serialize_11_2<std::string>::value);
	static_assert(!Has_serialize_11_2<A>::value);
	static_assert(Has_serialize_11_2<B>::value);
	static_assert(!Has_serialize_11_2<C>::value);

	std::cout << serialize_g(a) << "\n";
	std::cout << serialize_g(b) << "\n";

	//assert(99 == len(D()));

	//debug_type<containted_type_t<std::vector<std::string>>>();
	std::cout << "SFINAE test successful\n";
	return 0;
}