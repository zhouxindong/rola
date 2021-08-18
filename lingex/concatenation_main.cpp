#include "concatenation.hpp"
#include <iostream>
#include <functional>
#include <algorithm>
#include <iterator>

static bool begins_with_a(const std::string& s)
{
	return s.find("a") == 0;
}

static bool ends_with_b(const std::string& s)
{
	return s.rfind("b") == s.length() - 1;
}

static void f(int, double)
{
	std::cout << "f() \n";
}

static void g(int, double)
{
	std::cout << "g() \n";
}

static void h(int, double)
{
	std::cout << "h() \n";
}

static void bar(int a)
{
	std::cout << "bar(" << a << ")\n";
}

int main_concatenation()
{
	auto twice([](int i) {return i * 2; });
	auto thrice([](int i) {return i * 3; });

	auto combined(rola::concat_callable(twice, thrice, std::plus<int>{})); // twice(thrice(plus(a, b)))
	std::cout << combined(2, 3) << "\n";

	auto a_and_b(rola::combine(std::logical_and<>{}, begins_with_a, ends_with_b));
	std::vector<std::string> v{ "ac", "cb", "ab", "axxxb" };
	std::vector<std::string> result;
	std::copy_if(std::begin(v), std::end(v), std::back_inserter(result), a_and_b);
	std::copy(std::begin(result), std::end(result), std::ostream_iterator<std::string>{std::cout, ", "});
	std::cout << "\n";

	auto fgh = rola::multi_call(f, g, h);
	fgh(3, 3.2);

	rola::call_each(bar, 1, 2, 3, 4, 5);

	std::cout << "concatenation_main.cpp successful\n";
	return 0;
}