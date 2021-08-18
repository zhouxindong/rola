#include <iostream>
#include <variant>
#include <list>
#include <string>
#include <algorithm>
#include "inherience.h"

#pragma region demo

class cat
{
	std::string name;

public:
	cat(std::string n)
		: name{ n }
	{}

	void meow() const
	{
		std::cout << name << " says Meow!\n";
	}
};

class dog
{
	std::string name;

public:
	dog(std::string n)
		: name{ n }
	{}

	void woof() const
	{
		std::cout << name << " says Woof!\n";
	}
};

using animal = std::variant<dog, cat>;

// whether a variant object hold T type
template <typename T>
bool is_type(const animal& a)
{
	return std::holds_alternative<T>(a);
}

struct animal_voice
{
	void operator()(const dog& d) const { d.woof(); }
	void operator()(const cat& c) const { c.meow(); }
};

int main_variant()
{
	std::list<animal> l{ cat{"Tuba"}, dog{"Balou"}, cat{"Bobby"} };

	for (const animal& a : l)
	{
		switch (a.index())
		{
		case 0:
			std::get<dog>(a).woof();
			break;

		case 1:
			std::get<cat>(a).meow();
			break;
		}
	}
	std::cout << "---------------------------\n";

	for (const animal& a : l)
	{
		if (const auto d(std::get_if<dog>(&a)); d)
		{
			d->woof();
		}
		else if (const auto c(std::get_if<cat>(&a)); c)
		{
			c->meow();
		}
	}
	std::cout << "----------------------------\n";

	for (const animal& a : l)
	{
		std::visit(animal_voice{}, a);
	}
	std::cout << "----------------------------\n";

	std::cout << "There are "
		<< std::count_if(std::begin(l), std::end(l), is_type<cat>)
		<< " cats and "
		<< std::count_if(std::begin(l), std::end(l), is_type<dog>)
		<< " dogs in the list.\n";


	std::cout << "variant_main.cpp successful\n";
	return 0;
}

#pragma endregion

#pragma region usage

struct foo
{
	int value;
	explicit foo(int const i)
		: value(i) {}
};

void _f()
{
	std::variant<int, std::string, foo> v = 42; // holds int
	v.emplace<foo>(42);
}

void _f2()
{
	std::variant<int, double, std::string> v = 42;
	auto i1 = std::get<int>(v);
	auto i2 = std::get<0>(v);

	try
	{
		auto f = std::get<double>(v);
	}
	catch (std::bad_variant_access const& e)
	{
		std::cout << e.what() << std::endl;
	}
}

void _f3()
{
	std::variant<int, double, std::string> v;
	v = 42;		// int
	static_assert(std::variant_size_v<decltype(v)> == 3);
	std::cout << "index  = " << v.index() << std::endl;
	v = 42.0;	// double
	std::cout << "index = " << v.index() << std::endl;
	v = "42";	// string
	std::cout << "index = " << v.index() << std::endl;
}

void _f4()
{
	std::variant<int, double, std::string> v = 42;
	std::cout << "int? " << std::boolalpha
		<< std::holds_alternative<int>(v) << std::endl;
}

void _f5()
{
	std::variant<std::monostate, foo, int> v; // use std::monostate to first alternative is not default ctor
	v = 42;
	v = foo{ 42 };
}

#pragma endregion

#pragma region visting a std::variant

/*
a visitor is a callable object that accepts every possible alternative from a variant
visitation is done by invoking std::visit() with the visitor and one or more variant objects
*/

enum class Genre
{
	Drama, Action, SF, Comedy
};

#include <chrono>
#include <vector>

struct Movie
{
	std::string title;
	std::chrono::minutes length;
	std::vector<Genre> genre;
};

struct Track
{
	std::string title;
	std::chrono::seconds length;
};

struct Music
{
	std::string title;
	std::string artist;
	std::vector<Track> tracks;
};

struct Software
{
	std::string title;
	std::string vendor;
};

using dvd = std::variant<Movie, Music, Software>;

// 1. a void visitor that does not return anything
void _visitor1(const std::vector<dvd>& dvds)
{
	for (auto const& d : dvds)
	{
		std::visit([](auto&& arg)
			{
				std::cout << arg.title << std::endl;
			}, d);
	}
}



// 2. a visitor that returns a value; the value should have the same type regardless of the
// current alternative of the variant, or can be itself a variant
void _visitor2(const std::vector<dvd>& dvds)
{
	for (auto const& d : dvds)
	{
		dvd result = std::visit([](auto&& arg) -> dvd
			{
				auto cpy{ arg };
				//cpy.title = std::to_upper(cpy.title);
				return cpy;
			}, d);

		std::visit([](auto&& arg) {
			std::cout << arg.title << std::endl;
			}, result);
	}
}

// 3. a visitor that does type matching (which can either be a void or a value-returning visitor)
// implemented by providing a function object that has an overloaded call operator for each
// alternative type of the variant
struct visitor_functor
{
	void operator()(Movie const& arg) const
	{
	}

	void operator()(Music const& arg) const
	{}

	void operator()(Software const& arg) const
	{}
};

void _visitor3(const std::vector<dvd>& dvds)
{
	for (auto const& d : dvds)
	{
		std::visit(visitor_functor(), d);
	}
}

// 4. lambda
void _visitor4(const std::vector<dvd>& dvds)
{
	for (auto const& d : dvds)
	{
		std::visit([](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, Movie>)
				{

				}
				else if constexpr (std::is_same_v<T, Music>)
				{

				}
				else if constexpr (std::is_same_v<T, Software>)
				{

				}
			}, d);
	}
}
#pragma endregion