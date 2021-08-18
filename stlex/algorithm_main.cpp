#include "algorithm.hpp"
#include <iostream>
#include <string>
#include <list>
#include <iterator>
#include <algorithm>
#include <cassert>

int main_algorithm()
{
	// split
	const std::string s{ "a-b-c-d-e-f-g" };
	auto binfunc([](auto it_a, auto it_b) {
		return std::string(it_a, it_b);
		});

	std::list<std::string> l;
	rola::split(std::begin(s), std::end(s), std::back_inserter(l), '-', binfunc);
	std::copy(std::begin(l), std::end(l), std::ostream_iterator<std::string>(std::cout, "\n"));

	// gather
	auto is_a([](char c) {return c == 'a'; });
	std::string a{ "a_a_a_a_a_a_a_a_a_a" };
	auto middle(std::begin(a) + a.size() / 2);
	rola::gather(std::begin(a), std::end(a), middle, is_a);
	std::cout << a << '\n';

	rola::gather(std::begin(a), std::end(a), std::begin(a), is_a);
	std::cout << a << '\n';

	rola::gather(std::begin(a), std::end(a), std::end(a), is_a);
	std::cout << a << '\n';

	// remove_multi_whitespace
	std::string s2{ "fooo     bar    \t      baz" };
	std::cout << s2;
	s2.erase(rola::remove_multi_whitespace(std::begin(s2), std::end(s2)), std::end(s2));
	std::cout << s2;

	auto vnums = std::vector<int>{ 0,2,-3,5,-1,6,8,-4,9 };
	auto r = rola::mapf([](int const i) {
		return std::abs(i);
		}, vnums);

	auto words = std::map<std::string, int>{ {"one", 1},{"two", 2},{"three",3} };
	auto m = rola::mapf([](std::pair<std::string, int> const kvp) {
		return std::make_pair(rola::mapf(toupper, kvp.first), kvp.second);
		}, words);

	auto s1 = rola::foldl([](const int s, const int n) {
		return s + n;
		}, vnums, 0);
	auto s3 = rola::foldl(std::plus<>(), vnums, 0);
	assert(s1 == s3);

	std::cout << "\nalgorithm_main successful\n";
	return 0;
}