#include "rx_range.hpp"

#include <iostream>
#include <cassert>
#include <string>
#include <unordered_map>

using namespace rx;

int main_rrm1()
{
	//for (auto x : seq() | filter([](int x) {return x % 2 == 1; }) | first_n(15))
	//{
	//	std::cout << x << "\n";
	//}

	//auto ints = seq() | filter([](int x) {return x % 2 == 1; });
	//auto strings = ints | transform([](int x) {return std::to_string(x); });
	//auto map = zip(ints, strings) | first_n(15) | to_map();
	//assert(map.size() == 15);

	auto ints = std::vector{ {4,1,6,2,7,4} };
	auto strings = ints | transform([](int x) {return std::to_string(x); });
	auto sorted = strings | sort() | to_vector();
	assert(sorted.size() == 6);
	assert(sorted[0] == "1");
	assert(sorted[1] == "2");
	assert(sorted[2] == "4");
	assert(sorted[3] == "4");
	assert(sorted[4] == "6");
	assert(sorted[5] == "7");

	std::cout << "rx_range_main.cpp successful\n";
	return 0;
}

// custom combinators
struct convert_to_string
{
	template <typename Input>
	struct Range
	{
		using output_type = std::string;
		static constexpr bool is_finite = rx::is_finite_v<Input>;
		static constexpr bool is_idempotent = rx::is_idempotent_v<Input>;

		Input input;

		constexpr explicit Range(Input input)
			: input(std::move(input))
		{}

		[[nodiscard]] constexpr output_type get() const noexcept
		{
			return std::to_string(input.get());
		}

		constexpr void next() noexcept
		{
			input.next();
		}

		[[nodiscard]] constexpr bool at_end() const noexcept
		{
			return input.at_end();
		}

		[[nodiscard]] constexpr size_t size_hint() const noexcept
		{
			return input.size_hint();
		}

		constexpr size_t advance_by(size_t n) const noexcept
		{
			return rx::advance_by(input, n);
		}
	};

	template <typename Input>
	[[nodiscard]] constexpr auto operator()(Input&& input) const
	{
		using Inner = decltype(rx::as_input_range(std::forward<Input>(input)));
		return Range<Inner>(rx::as_input_range(std::forward<Input>(input)));
	}
};

std::vector<std::string> convert_ints_to_sorted_strings(std::vector<int> input)
{
	return input | convert_to_string() | rx::sort() | rx::to_vector();
}

// custom sink
struct normalize
{
	template <typename Input>
	struct Range
	{
		using output_type = rx::get_output_type_of_t<Input>;

		Input input;
		constexpr explicit Range(Input input)
			: input(std::move(input))
		{}

		template <typename Out>
		constexpr void sink(Out& out) && noexcept
		{
			rx::sink(std::move(input), out);

			auto square = [](auto x) {return x * x; };
			auto length = std::sqrt(out | transform(square) | sum());

			for (auto& x : out)
			{
				x /= length;
			}
		}
	};

	template <typename Input>
	[[nodiscard]] constexpr auto operator()(Input&& input) const
	{
		using Inner = rx::remove_cvref_t<Input>;
		return Range<Inner>(std::forward<Input>(input));
	}
};

std::vector<double> normalize_vector(std::vector<double> input)
{
	return input | normalize() | to_vector();
}

// custom aggragators
//struct average
//{
//	template <typename Input>
//	constexpr auto operator()(Input&& input) const
//	{
//		using element_type = rx::get_idempotent_range_type_t<Input>;
//		auto [count, summed] = rx::zip(rx::seq(1, 0), input)
//			| rx::foldl(std::tuple(size_t(0), element_type{ 0 }),
//				[](auto&& accum, auto&& element) {
//					return std::tuple(std::get<0>(accum) + std::get<0>(element),
//						std::get<1>(accum) + std::get<1>(element));
//				});
//		return summed / element_type(count);
//	}
//};
//
//double compute_average(std::vector<double> values)
//{
//	return values | average();
//}

template <typename T>
std::string to_string(T val)
{
	return std::to_string(val);
}

//template <typename T, typename U>
//bool operator==(std::vector<T> const& lhs, std::vector<U> const& rhs)
//{
//	if (lhs.size() != lhs.size())
//		return false;
//	for (size_t i = 0; i < lhs.size(); ++i)
//	{
//		if (lhs[i] != rhs[i])
//			return false;
//	}
//	return true;
//}

int main_rx_range()
{
	// advance overflow
	auto bounds = seq() | take(10);
	advance_by(bounds, 11);
	assert(bounds.i == bounds.n);

	// transform
	auto strings = std::vector{ 1,2,3,4 } | transform(&to_string<int>)
		| to_vector();
	assert((strings == std::vector<std::string>{"1", "2", "3", "4"}));

	// filter
	auto odd = std::list{ 1,2,3,4 } | filter([](int x) {return x % 2 == 1; })
		| to_list();
	assert((odd == std::list{ 1,3 }));

	// first
	auto morty = std::vector{ {"Hello", "World", "Morty"} }
	| filter([](std::string_view sv) {return sv.find('y') != std::string::npos; })
		| first();
	assert(morty);
	assert(std::string(*morty) == "Morty");

	// first_n
	auto first_3 = std::vector{ {1,2,3,4,5} } | first_n(3) | to_vector();
	assert((first_3 == std::vector{ 1,2,3 }));

	// skip_n
	auto input = seq() | skip_n(1000) | first_n(10) | to_vector();
	auto expected = seq(1000) | first_n(10) | to_vector();
	assert((input == expected));

	// zip
	auto input1 = seq() | first_n(5);
	auto input2 = input1 | transform(&to_string<int>);
	auto input3 = seq(10);
	auto zipped = zip(input1, input2, input3) | to_vector();
	assert(zipped.size() == 5);
	//auto expected2 = std::vector{
	//	std::make_tuple(0, "0", 10),
	//	std::make_tuple(1, "1", 11),
	//	std::make_tuple(2, "2", 12),
	//	std::make_tuple(3, "3", 13),
	//	std::make_tuple(4, "4", 14),
	//};
	//assert((zipped == expected2));

	// advance_by & get
	auto input4 = zip(seq(), seq(1));
	advance_by(input4, 10);
	assert((input4.get() == std::tuple{ 10, 11 }));

	// to_set
	auto input5 = std::vector{ {0,0,1,1} };
	auto result = as_input_range(input5) | to_set();
	assert(result.size() == 2);
	assert((result == std::set{ {0, 1} }));

	// range append to container
	std::unordered_map<double, std::string> result2;
	auto keys = seq();
	auto values = keys | transform(&to_string<int>);
	zip(keys, values) | first_n(5) | append(result2);
	auto expected2 = std::unordered_map<double, std::string>
	{
		{std::make_pair(0.0, "0"),
		std::make_pair(1.0, "1"),
		std::make_pair(2.0, "2"),
		std::make_pair(3.0, "3"),
		std::make_pair(4.0, "4")}
	};
	assert((result2 == expected2));

	// range append to rvalue container
	auto lower = seq<char>('a') | take(26) | append(std::vector<char>());
	assert(lower.size() == 26);

	// generate
	int x = 0;
	auto input6 = generate([&] {return x++; });
	auto result3 = input6 | first_n(5) | to_vector();
	auto expected3 = seq() | first_n(5) | to_vector();
	assert((result3 == expected3));

	// until
	auto input7 = seq() | until([](int x) {return x == 5; });
	auto result4 = input7 | to_vector();
	auto expected4 = seq() | first_n(5) | to_vector();
	assert((result4 == expected4));

	// any_of
	auto input8 = seq() | first_n(5);
	auto a = input | any_of([](int x) {return x > 3; });
	assert(a);
	auto b = input | any_of([](int x) {return x == 5; });
	assert(!b);

	// all_of
	auto a2 = input8 | all_of([](int x) {return x < 5; });
	assert(a2);
	auto b2 = input8 | all_of([](int x) {return x < 4; });
	assert(!b2);

	// none_of
	auto a3 = input8 | none_of([](int x) {return x > 4; });
	assert(a3);
	auto b3 = input8 | none_of([](int x) {return x == 4; });
	assert(!b3);

	// count
	auto a4 = input8 | count();
	assert(a4 == 5);

	// fill
	std::string a5;
	fill_n(5, 'a') | append(a5);
	assert(a5 == std::string("aaaaa"));

	// sum
	int v = 7;
	assert((fill(v) | take(5) | sum()) == 7 * 5);

	// max, min
	auto s = seq() | first_n(5) | max();
	assert(*s == 4);
	auto s2 = seq() | first_n(5) | min();
	assert(*s2 == 0);

	// sort
	auto sorted = std::vector{ {3,2,1} } | sort() | to_vector();
	assert(std::is_sorted(begin(sorted), end(sorted)));

	// reverse
	auto input9 = std::vector{ {2,3,6,1,7,8,3,4} };
	auto result9 = input9 | sort() | reverse() | to_vector();
	auto expected9 = std::vector{ {8,7,6,4,3,3,2,1} };
	assert((result9 == expected9));

	

	std::cout << "rx_range_main.cpp successful\n";
	return 0;
}