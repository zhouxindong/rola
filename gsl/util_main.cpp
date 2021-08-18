#include <iostream>
#include "util.hpp"

#include "util.hpp"    // finally, narrow_cast
#include <algorithm>   // for move
#include <functional>  // for reference_wrapper, _Bind_helper<>::type
#include <limits>      // for numeric_limits
#include <stdint.h>    // for uint32_t, int32_t
#include <type_traits> // for is_same
#include <cstddef>     // for std::ptrdiff_t
#include <cassert>

namespace
{
	void f(int& i)
	{
		i += 1;
	}

	static int j = 0;

	void g()
	{
		j += 1;
	}
}

int main_util()
{
	// sanity check for gsl::index typedef
	{
		static_assert(std::is_same_v<gsl::index, std::ptrdiff_t>);
	}

	// finally lambda
	{
		int i = 0;
		{
			auto _ = gsl::finally([&]() {
				f(i);
				});

			assert(i == 0);
		}
		assert(i == 1);
	}

	// finally lambda move
	{
		int i = 0;
		{
			auto _1 = gsl::finally([&]() {
				f(i);
				});
			{
				auto _2 = std::move(_1);
				assert(i == 0);
			}
			assert(i == 1);
			{
				auto _2 = std::move(_1);
				assert(i == 1);
			}
			assert(i == 1);
		}
		assert(i == 1);
	}

	// finally const lvalue lambda
	{
		int i = 0;
		{
			const auto const_lvalue_lambda = [&]() {f(i); };
			auto _ = gsl::finally(const_lvalue_lambda);
			assert(i == 0);
		}
		assert(i == 1);
	}

	// finally mutable lvalue lambda
	{
		int i = 0;
		{
			auto mutable_lvalue_lambda = [&]() {f(i); };
			auto _ = gsl::finally(mutable_lvalue_lambda);
			assert(i == 0);
		}
		assert(i == 1);
	}

	// finally function with bind
	{
		int i = 0;
		{
			auto _ = gsl::finally(std::bind(&f, std::ref(i)));
			assert(i == 0);
		}
		assert(i == 1);
	}

	// finally function ptr
	{
		j = 0;
		{
			auto _ = gsl::finally(&g);
			assert(j == 0);
		}
		assert(j == 1);
	}

	// narrow cast
	{
		int n = 120;
		char c = gsl::narrow_cast<char>(n);
		assert(c == 120);

		n = 300;
		unsigned char uc = gsl::narrow_cast<unsigned char>(n);
		assert(uc == 44);
	}

	// narrow
	{
		int n = 120;
		const char c = gsl::narrow<char>(n);
		assert(c == 120);

		n = 300;
		//gsl::narrow<char>(n);	// exception!

		const auto int32_max = std::numeric_limits<int32_t>::max();
		const auto int32_min = std::numeric_limits<int32_t>::min();
	}

	// at
	{
		int a[4] = { 1,2,3,4 };
		const int(&c_a)[4] = a;
		for (int i = 0; i < 4; ++i)
		{
			assert(&gsl::at(a, i) == &a[i]);
			assert(&gsl::at(c_a, i) == &a[i]);
		}

		//gsl::at(a, -1); // error
		//gsl::at(a, 4);	// error
		//gsl::at(c_a, -1);	// error
		//gsl::at(c_a, 4);	// error
	}

	// std array
	{
		std::array<int, 4> a = { 1,2,3,4 };
		const std::array<int, 4>& c_a = a;

		for (int i = 0; i < 4; ++i)
		{
			assert(&gsl::at(a, i) == &a[static_cast<std::size_t>(i)]);
			assert(&gsl::at(c_a, i) == &a[static_cast<std::size_t>(i)]);
		}
	}

	// std vector
	{
		std::vector<int> a = { 1,2,3,4 };
		const std::vector<int>& c_a = a;

		for (int i = 0; i < 4; ++i)
		{
			assert(&gsl::at(a, i) == &a[static_cast<std::size_t>(i)]);
			assert(&gsl::at(c_a, i) == &a[static_cast<std::size_t>(i)]);
		}
	}

	// initializer list
	{
		const std::initializer_list<int> a = { 1,2,3,4 };

		for (int i = 0; i < 4; ++i)
		{
			assert(gsl::at(a, i) == i + 1);
			assert(gsl::at({ 1,2,3,4 }, i) == i + 1);
		}
	}

	// constexpr
	{
		int a1[4] = { 1,2,3,4 };
		const int(&c_a1)[4] = a1;
		std::array<int, 4> a2 = { 1,2,3,4 };
		const std::array<int, 4>& c_a2 = a2;

		for (int i = 0; i < 4; ++i)
		{
			assert(&gsl::at(a1, i) == &a1[i]);
			assert(&gsl::at(c_a1, i) == &a1[i]);
			assert(&gsl::at(c_a2, i) == &c_a2[static_cast<std::size_t>(i)]);
			assert(gsl::at({ 1,2,3,4 }, i) == i + 1);
		}
	}
	std::cout << "util_main successful\n";
	return 0;
}