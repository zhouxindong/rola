#include <tuple>
#include <iostream>
#include <string>
#include <utility>
#include <type_traits>

// three ways to build a tuple
// 1. std::make_tuple (if you need values in the returned tuple)
// 2. std::tie        (if you need lvalue references in the returned tuple)
// 3. std::forward_as_tuple (if you if you need to keep the types of references of the inputs to build the tuple)

// error
template <typename... Ts>
class X
{
public:
	explicit X(Ts const&... values)
		: values_(std::make_tuple(values...))
	{}
	std::tuple<Ts const&...> values_;
};

// can be work
template <typename... Ts>
class X2
{
public:
	explicit X2(Ts const&... values)
		: values_(std::make_tuple(std::ref(values)...))
	{}
	std::tuple<Ts const&...> values_;
};

// correctly select
template <typename... Ts>
class X3
{
public:
	explicit X3(Ts const&... values)
		: values_(std::tie(values...))
	{}
	std::tuple<Ts const&...> values_;
};

std::string universe()
{
	return "universe";
}

int main()
{
	int i = 42;
	auto s = std::string("universe");

	// Exception!!
	//auto x = X<int, std::string>(i, s);
	//std::cout << "i = " << std::get<0>(x.values_) << "\n";
	//std::cout << "s = " << std::get<1>(x.values_) << "\n";

	auto x2 = X2<int, std::string>(i, s);
	std::cout << "i = " << std::get<0>(x2.values_) << "\n";
	std::cout << "s = " << std::get<1>(x2.values_) << "\n";

	auto x3 = X3<int, std::string>(i, s);
	std::cout << "i = " << std::get<0>(x3.values_) << "\n";
	std::cout << "s = " << std::get<1>(x3.values_) << "\n";

	auto myTuple = std::forward_as_tuple(i, universe()); // <int&, std::string&&>
	static_assert(std::is_same_v<decltype(myTuple),
		std::tuple<int&, std::string&&>>);

	std::cout << "tuple_main successful\n";
	return 0;
}