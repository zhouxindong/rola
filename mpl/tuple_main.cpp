#include "tuple.hpp"
#include "string"
#include <cassert>
#include <iostream>

int main_tm()
{
	// ctor
	rola::mpl::Tuple<int, double, std::string> t(17, 3.14, "hello world");
	
	// get
	assert(rola::mpl::get<0>(t) == 17);
	assert(3.14 == rola::mpl::get<1>(t));
	assert(rola::mpl::get<2>(t) == std::string("hello world"));

	rola::mpl::Tuple<long int, long double, std::string> t2(t);
	assert((long int)17 == rola::mpl::get<0>(t2));
	assert((long double)3.14 == rola::mpl::get<1>(t2));
	assert(rola::mpl::get<2>(t2) == std::string("hello world"));

	//[]
	auto ctv0 = rola::CT_value<unsigned, 0>();
	auto ctv1 = rola::CT_value<unsigned, 1>();
	auto ctv2 = rola::CT_value<unsigned, 2>();
	assert((long int)17 == t2[ctv0]);
	assert((long double)3.14 == t2[ctv1]);
	assert(t2[ctv2] == std::string("hello world"));

	// make_tuple
	auto t3 = rola::mpl::make_tuple(17, 3.14, "hello world");
	assert(17 == rola::mpl::get<0>(t3));
	assert(3.14 == rola::mpl::get<1>(t3));
	assert(rola::mpl::get<2>(t3) == std::string("hello world"));

	auto t4 = rola::mpl::make_tuple(0, '1', 2.2f, std::string{ "hello" });
	assert((t4[rola::CT_value<unsigned, 0>{}] == 0));
	assert((t4[rola::CT_value<unsigned, 1>{}] == '1'));
	assert((t4[rola::CT_value<unsigned, 2>{}] == 2.2f));
	assert((t4[rola::CT_value<unsigned, 3>{}] == std::string{ "hello" }));

	std::cout << "tuple_main run successful\n";
	return 0;
}