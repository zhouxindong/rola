#include "type_list.hpp"
#include <type_traits>

#include <iostream>
#include <cassert>

class Base {};
class Derived1 : public Base {};
class Derived2 : public Base {};
class Derived11 : public Derived1 {};

using empty_typelist = rola::Type_list<>;
using integer_typelist = rola::Type_list<char, short, unsigned short,
	int, unsigned int, long long, unsigned long long>;
using float_typelist = rola::Type_list<float, double>;
using string_typelist = rola::Type_list<std::string>;

template <typename T>
struct Add_const
{
	using type = T const;
};

template <typename T>
struct Add_const<T const>
{
	using type = T const;
};

template <typename T, typename U>
class Larger_type
{
public:
	using type = std::conditional_t<(sizeof(T) >= sizeof(U)), T, U>;
};

template <typename T>
class Size4
{
public:
	static constexpr bool value = (sizeof(T) == 4);
};

template <typename T>
struct Holder
{
	T value_;
};

int main_tlm()
{
	// Length
	static_assert(0 == rola::TL::Length_v<empty_typelist>);
	static_assert(1 == rola::TL::Length_v<string_typelist>);
	static_assert(2 == rola::TL::Length_v<float_typelist>);
	static_assert(7 == rola::TL::Length_v<integer_typelist>);

	// Front
	static_assert(std::is_same_v<float, rola::TL::Front_t<float_typelist>>);
	static_assert(std::is_same_v<void, rola::TL::Front_t<empty_typelist>>);
	static_assert(std::is_same_v<std::string, rola::TL::Front_t<string_typelist>>);

	// Pop_front
	static_assert(std::is_same_v<
		rola::Type_list<short, unsigned short,
		int, unsigned int, long long, unsigned long long>,
		rola::TL::Pop_front_t<integer_typelist>>);
	static_assert(std::is_same_v<
		rola::Type_list<double>, rola::TL::Pop_front_t<float_typelist>>);
	static_assert(std::is_same_v<rola::Type_list<>, rola::TL::Pop_front_t<string_typelist>>);

	// Push_front
	static_assert(std::is_same_v<
		rola::Type_list<double>, rola::TL::Push_front_t<rola::Type_list<>, double>>);
	static_assert(std::is_same_v<
		rola::Type_list<int, std::string>, rola::TL::Push_front_t<string_typelist, int>>);


	// N_element
	static_assert(std::is_same_v<std::string, rola::TL::N_element_t<string_typelist, 0>>);
	static_assert(std::is_same_v<double,
		rola::TL::N_element_t<float_typelist, 1>>);
	static_assert(std::is_same_v<void,
		rola::TL::N_element_t<float_typelist, 2>>); // out of range
	static_assert(std::is_same_v<long long,
		rola::TL::N_element_t<integer_typelist, 5>>);

	// Index_of
	static_assert(-1 == rola::TL::Index_of_v<empty_typelist, int>);
	static_assert(0 == rola::TL::Index_of_v<string_typelist, std::string>);
	static_assert(-1 == rola::TL::Index_of_v<string_typelist, int>);
	static_assert(1 == rola::TL::Index_of_v<float_typelist, double>);
	static_assert(4 == rola::TL::Index_of_v<integer_typelist, unsigned int>);

	// Largest_type
	static_assert(std::is_same_v<char, rola::TL::Largest_type_t<empty_typelist>>);
	static_assert(std::is_same_v<std::string, rola::TL::Largest_type_t<string_typelist>>);
	static_assert(std::is_same_v<double, rola::TL::Largest_type_t<float_typelist>>);
	static_assert(std::is_same_v<long long, rola::TL::Largest_type_t<integer_typelist>>);

	// Is_empty
	static_assert(rola::TL::Is_empty_v<empty_typelist>);
	static_assert(!rola::TL::Is_empty_v<integer_typelist>);
	static_assert(!rola::TL::Is_empty_v<float_typelist>);
	static_assert(!rola::TL::Is_empty_v<string_typelist>);

	// Push_back
	static_assert(std::is_same_v<
		rola::Type_list<char>,
		rola::TL::Push_back_t<empty_typelist, char>>);

	static_assert(std::is_same_v<
		rola::Type_list<std::string, int>,
		rola::TL::Push_back_t<string_typelist, int>>);

	static_assert(std::is_same_v<
		rola::Type_list<float, double, short>,
		rola::TL::Push_back_t<float_typelist, short>>);

	// Back
	static_assert(std::is_same_v<void, rola::TL::Back_t<empty_typelist>>);
	static_assert(std::is_same_v<std::string, rola::TL::Back_t<string_typelist>>);
	static_assert(std::is_same_v<double, rola::TL::Back_t<float_typelist>>);
	static_assert(std::is_same_v<unsigned long long, rola::TL::Back_t<integer_typelist>>);

	// Pop_back
	static_assert(std::is_same_v<rola::Type_list<>, rola::TL::Pop_back_t<empty_typelist>>);
	static_assert(std::is_same_v<rola::Type_list<>, rola::TL::Pop_back_t<string_typelist>>);
	static_assert(std::is_same_v<rola::Type_list<float>, rola::TL::Pop_back_t<float_typelist>>);
	static_assert(std::is_same_v<
		rola::Type_list<char, short>,
		rola::TL::Pop_back_t<rola::Type_list<char, short, unsigned short>>>);
	static_assert(std::is_same_v<
		rola::Type_list<char, short, unsigned short, int, unsigned int, long long>,
		rola::TL::Pop_back_t<integer_typelist>>);

	// Erase
	static_assert(std::is_same_v<empty_typelist, rola::TL::Erase_t<empty_typelist, int>>);
	static_assert(std::is_same_v<empty_typelist, rola::TL::Erase_t<string_typelist, std::string>>);
	static_assert(std::is_same_v<string_typelist, rola::TL::Erase_t<string_typelist, int>>);
	static_assert(std::is_same_v<
		rola::Type_list<float>,
		rola::TL::Erase_t<float_typelist, double>
	>);
	static_assert(std::is_same_v<
		rola::Type_list<double>,
		rola::TL::Erase_t<float_typelist, float>
	>);
	static_assert(std::is_same_v<
		rola::Type_list<char, short, int, unsigned int, long long, unsigned long long>,
		rola::TL::Erase_t<integer_typelist, unsigned short>
	>);

	// Erase_all
	static_assert(std::is_same_v<empty_typelist, rola::TL::Erase_all_t<empty_typelist, char>>);
	static_assert(std::is_same_v<empty_typelist, rola::TL::Erase_all_t<string_typelist, std::string>>);
	static_assert(std::is_same_v<string_typelist, rola::TL::Erase_all_t<string_typelist, int>>);
	static_assert(std::is_same_v<
		rola::Type_list<float>,
		rola::TL::Erase_all_t<float_typelist, double>
	>);
	static_assert(std::is_same_v<
		rola::Type_list<double>,
		rola::TL::Erase_all_t<float_typelist, float>
	>);
	static_assert(std::is_same_v<
		rola::Type_list<char, short, int, unsigned int, long long, unsigned long long>,
		rola::TL::Erase_all_t<integer_typelist, unsigned short>
	>);
	static_assert(std::is_same_v<
		rola::Type_list<>,
		rola::TL::Erase_all_t<rola::Type_list<int, int>, int>
	>);

	static_assert(std::is_same_v<
		rola::Type_list<int, short, double>,
		rola::TL::Erase_all_t<rola::Type_list<char, int, short, char, double>, char>
	>);

	// Unique
	static_assert(std::is_same_v<
		rola::Type_list<>,
		rola::TL::Unique_t<empty_typelist>>);

	static_assert(std::is_same_v<
		rola::Type_list<int>,
		rola::TL::Unique_t<rola::Type_list<int>>>);

	static_assert(std::is_same_v<
		rola::Type_list<int>,
		rola::TL::Unique_t<rola::Type_list<int, int>>>);

	static_assert(std::is_same_v<
		rola::Type_list<int>,
		rola::TL::Unique_t<rola::Type_list<int, int, int>>>);

	static_assert(std::is_same_v<
		rola::Type_list<int, double, float>,
		rola::TL::Unique_t<rola::Type_list<int, double, float>>>);

	static_assert(std::is_same_v<
		rola::Type_list<int, std::string, double>,
		rola::TL::Unique_t<rola::Type_list<int, std::string, int, double, double, int>>>); (std::is_same_v<
		rola::Type_list<>,
		rola::TL::Unique_t<empty_typelist>>);

	static_assert(std::is_same_v<
		rola::Type_list<int>,
		rola::TL::Unique_t<rola::Type_list<int>>>);

	static_assert(std::is_same_v<
		rola::Type_list<int>,
		rola::TL::Unique_t<rola::Type_list<int, int>>>);

	static_assert(std::is_same_v<
		rola::Type_list<int>,
		rola::TL::Unique_t<rola::Type_list<int, int, int>>>);

	static_assert(std::is_same_v<
		rola::Type_list<int, double, float>,
		rola::TL::Unique_t<rola::Type_list<int, double, float>>>);

	static_assert(std::is_same_v<
		rola::Type_list<int, std::string, double>,
		rola::TL::Unique_t<rola::Type_list<int, std::string, int, double, double, int>>>);

	// Replace
	static_assert(std::is_same_v<
		empty_typelist,
		rola::TL::Replace_t<empty_typelist, int, double>>);

	static_assert(std::is_same_v<
		rola::Type_list<int>,
		rola::TL::Replace_t<string_typelist, std::string, int>>);

	static_assert(std::is_same_v<
		rola::Type_list<double, double>,
		rola::TL::Replace_t<float_typelist, float, double>>);

	static_assert(std::is_same_v<
		rola::Type_list<char, short, std::string, int, unsigned int, long long, unsigned long long>,
		rola::TL::Replace_t<integer_typelist, unsigned short, std::string>>);

	// Replace_all
	static_assert(std::is_same_v<
		empty_typelist,
		rola::TL::Replace_all_t<empty_typelist, int, double>>);

	static_assert(std::is_same_v<
		rola::Type_list<int>,
		rola::TL::Replace_all_t<string_typelist, std::string, int>>);

	static_assert(std::is_same_v<
		rola::Type_list<double, double>,
		rola::TL::Replace_all_t<float_typelist, float, double>>);

	static_assert(std::is_same_v<
		rola::Type_list<char, short, std::string, int, unsigned int, long long, unsigned long long>,
		rola::TL::Replace_all_t<integer_typelist, unsigned short, std::string>>);

	static_assert(std::is_same_v<
		rola::Type_list<float, std::string, float, double, char, float>,
		rola::TL::Replace_all_t<rola::Type_list<float, std::string, int, double, char, int>, int, float>>);

	// Reverse
	static_assert(std::is_same_v<
		empty_typelist,
		rola::TL::Reverse_t<empty_typelist>>);

	static_assert(std::is_same_v<
		string_typelist,
		rola::TL::Reverse_t<string_typelist>>);

	static_assert(std::is_same_v<
		rola::Type_list<double, float>,
		rola::TL::Reverse_t<float_typelist>>);

	static_assert(std::is_same_v<
		rola::Type_list<unsigned long long, long long, unsigned, int, unsigned short, short, char>,
		rola::TL::Reverse_t<integer_typelist>>);

	// Most_derived
	static_assert(std::is_same_v<
		Base,
		rola::TL::Most_derived_t<empty_typelist, Base>
	>);

	static_assert(std::is_same_v<
		Base,
		rola::TL::Most_derived_t<integer_typelist, Base>
	>);

	static_assert(std::is_same_v<
		Base,
		rola::TL::Most_derived_t<rola::Type_list<double, std::string, Base, int>, Base>
	>);

	static_assert(std::is_same_v<
		Derived2,
		rola::TL::Most_derived_t<rola::Type_list<int, Base, Derived1, Derived2>, Base>
	>);

	static_assert(std::is_same_v<
		Derived11,
		rola::TL::Most_derived_t<rola::Type_list<Derived11, Derived1, int>, Base>
	>);

	// Derived_to_front
	static_assert(std::is_same_v<
		empty_typelist,
		rola::TL::Derived_to_front_t<empty_typelist>
	>);

	static_assert(std::is_same_v<
		string_typelist,
		rola::TL::Derived_to_front_t<string_typelist>
	>);

	static_assert(std::is_same_v<
		float_typelist,
		rola::TL::Derived_to_front_t<float_typelist>
	>);

	static_assert(std::is_same_v<
		integer_typelist,
		rola::TL::Derived_to_front_t<integer_typelist>
	>);

	static_assert(std::is_same_v<
		rola::Type_list<Derived11, int, Base, Derived1>,
		rola::TL::Derived_to_front_t<rola::Type_list<Base, int, Derived11, Derived1>>
	>);

	// map (Transform)
	static_assert(std::is_same_v<
		empty_typelist,
		rola::TL::Transform_t<empty_typelist, Add_const>>);

	static_assert(std::is_same_v<
		rola::Type_list<std::string const>,
		rola::TL::Transform_t<string_typelist, Add_const>>);

	static_assert(std::is_same_v<
		rola::Type_list<float const, double const>,
		rola::TL::Transform_t<float_typelist, Add_const>>);

	static_assert(std::is_same_v<
		rola::Type_list<char const, short const, unsigned short const, int const,
		unsigned int const, long long const, unsigned long long const>,
		rola::TL::Transform_t<integer_typelist, Add_const>>);

	// reduce (accumulate)
	static_assert(std::is_same_v<
		char,
		rola::TL::Accumulate_t<empty_typelist, Larger_type, char>
	>);

	static_assert(std::is_same_v<
		std::string,
		rola::TL::Accumulate_t<string_typelist, Larger_type, char>
	>);

	static_assert(std::is_same_v<
		double,
		rola::TL::Accumulate_t<float_typelist, Larger_type, char>
	>);

	static_assert(std::is_same_v<
		long long,
		rola::TL::Accumulate_t<integer_typelist, Larger_type, char>
	>);

	// filter
	static_assert(std::is_same_v<
		empty_typelist,
		rola::TL::Filter_t<empty_typelist, Size4>
	>);

	static_assert(std::is_same_v<
		rola::Type_list<>,
		rola::TL::Filter_t<string_typelist, Size4>
	>);

	static_assert(std::is_same_v<
		rola::Type_list<float>,
		rola::TL::Filter_t<float_typelist, Size4>
	>);

	static_assert(std::is_same_v<
		rola::Type_list<int, unsigned int>,
		rola::TL::Filter_t<integer_typelist, Size4>
	>);

	// apply
	rola::TL::Apply<rola::Type_list<int, double, std::string>, Holder> info;

	int& ri = static_cast<Holder<int>&>(info).value_;
	double& rd = static_cast<Holder<double>&>(info).value_;
	std::string& rs = static_cast<Holder<std::string>&>(info).value_;

	rola::TL::field<int>(info).value_ = 323;
	rola::TL::field<double>(info).value_ = 99.323;
	rola::TL::field<std::string>(info).value_ = "hello, world";

	assert(ri == 323);
	assert(rd == 99.323);
	assert(rs == std::string("hello, world"));

	std::cout << "type_list test successful\n";

	return 0;
}