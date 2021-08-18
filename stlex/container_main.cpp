#include <vector>
#include <iostream>
#include <cassert>
#include <map>
#include <string>

#include "container.hpp"
#include "string.hpp"

class Bar
{
public:
	Bar()
	{
		std::cout << "Bar::Bar()\n";
	}
	Bar(Bar const&)
	{
		std::cout << "Bar::Bar(Bar const&)\n";
	}
	Bar(Bar&&) noexcept
	{
		std::cout << "Bar::Bar(Bar&&)\n";
	}
};

class Person
{
public:
	std::string name;
	int age;

public:
	Person(std::string n, int a)
		: name(n), age(a)
	{}

	Person(int a)
		: age(a)
	{
		name = rola::stlex::to_string(age);
	}

	Person()
		: name(""), age(0)
	{}
};

namespace rola
{
	namespace stlex
	{
		namespace detail
		{
			template <>
			struct Reduce_policy<Person>
			{
				using type = int;
				static constexpr int init_value = 0;
			};
		}
	}
}

class Addtwo
{
public:
	double operator()(int i) const
	{
		return i + 2.0;
	}
};
int main_container()
{
	std::vector<int> v{ 1,2,3,4,5,6 };
	rola::stlex::erase_remove(v, [](int const& i) {
		return i % 2 == 0;
		});
	assert(v.size() == 3);
	assert(v[0] == 1);
	assert(v[1] == 3);
	assert(v[2] == 5);

	std::map<std::string, int> m{ {"1", 1}, {"2", 2}, {"3", 3} };
	rola::stlex::replace_key(m, "2", "two");
	assert(m["1"] == 1);
	assert(m["two"] == 2);
	assert(m["3"] == 3);
	assert(m.size() == 3);

	v = { 1,2,3,4,5,6 };
	auto v2 = rola::stlex::detail::pipe_map(v, [](auto&& a) {
		return a * 2;
		});
	assert(v2.size() == 6);
	assert(v2[0] == 2);
	assert(v2[1] == 4);
	assert(v2[2] == 6);
	assert(v2[3] == 8);
	assert(v2[4] == 10);
	assert(v2[5] == 12);

	auto vv2 = rola::stlex::detail::pipe_map
		(v, [](auto&& a) {
		return rola::stlex::to_string(a);
			});
	assert(vv2.size() == 6);
	assert(vv2[0] == std::string("1"));
	assert(vv2[1] == std::string("2"));
	assert(vv2[2] == std::string("3"));
	assert(vv2[3] == std::string("4"));
	assert(vv2[4] == std::string("5"));
	assert(vv2[5] == std::string("6"));

	Bar bar1;
	Bar bar2;
	std::vector<Bar> bs{ bar1, bar2 };
	auto vv3 = rola::stlex::detail::pipe_map(
		bs, [](auto&& b) { return 0; }
	);

	auto v3 = rola::stlex::detail::pipe_filter(v, [](int a) {
		return a % 2 == 0;
		});
	assert(v3.size() == 3);
	assert(v3[0] == 2);
	assert(v3[1] == 4);
	assert(v3[2] == 6);

	std::vector<Person> persons{ Person("zhang3", 35), Person("li4", 53) };
	auto vv4 = rola::stlex::detail::pipe_filter(persons, [](auto&& p) {
		return p.age > 50;
		});
	assert(vv4.size() == 1);
	assert(vv4[0].age == 53);

	auto sumv3 = rola::stlex::detail::reduce(v3, [](long o, int a) {
		return o + a;
		});
	assert(sumv3 == 12);

	auto allpersons = rola::stlex::detail::reduce(persons, [](int age, Person const& p) {
		return age + p.age; });
	assert(allpersons == 88);

	using namespace rola::stlex::detail;
	auto doubleitem = [](auto&& a) {
		return a * 2;
	};

	using namespace rola::stlex;
	auto v6 = v | doubleitem;

	assert(v6.size() == 6);
	assert(v6[0] == 2);
	assert(v6[1] == 4);
	assert(v6[2] == 6);
	assert(v6[3] == 8);
	assert(v6[4] == 10);
	assert(v6[5] == 12);

	auto int2str = [](auto&& a) {
		return rola::stlex::to_string(a);
	};
	auto v7 = v | int2str;
	assert(v7.size() == 6);
	assert(v7[0] == std::string("1"));
	assert(v7[1] == std::string("2"));
	assert(v7[2] == std::string("3"));
	assert(v7[3] == std::string("4"));
	assert(v7[4] == std::string("5"));
	assert(v7[5] == std::string("6"));

	auto v8 = v | doubleitem | int2str;
	assert(v8.size() == 6);
	assert(v8[0] == std::string("2"));
	assert(v8[1] == std::string("4"));
	assert(v8[2] == std::string("6"));
	assert(v8[3] == std::string("8"));
	assert(v8[4] == std::string("10"));
	assert(v8[5] == std::string("12"));

	auto odd = [](int a) {
		return a % 2 == 0;
	};
	v3 = v > odd;
	assert(v3.size() == 3);
	assert(v3[0] == 2);
	assert(v3[1] == 4);
	assert(v3[2] == 6);

	auto old50 = [](auto&& p) {
		return p.age > 50;
	};
	vv4 = persons > old50;
	assert(vv4.size() == 1);
	assert(vv4[0].age == 53);

	auto sum = [](long o, int a) {
		return o + a;
	};
	sumv3 = v3 >> sum;
	assert(sumv3 == 12);

	auto sumage = [](int age, Person const& p) {
		return age + p.age; };
	auto sage = persons >> sumage;
	assert(sage == 88);

	auto toperson = [](auto&& a) {
		return Person(a);
	};

	std::vector<int> vw{ 1,2,3,4,5,6,7,8,9 };
	auto finl = (((vw | doubleitem) > odd) | toperson) >> sumage;
	assert(finl == 90);

	std::cout << "container_main.cpp successful\n";
	return 0;
}