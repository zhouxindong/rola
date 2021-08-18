#include <iostream>
#include "span_ext.hpp"
#include <cassert>

int main()
{
	int arr[4] = { 1,2,3,4 };

	// from pointer length ctor
	{
		auto s = gsl::make_span(&arr[0], 2);
		assert(s.size() == 2);
		assert(s.data() == &arr[0]);
		assert(s[0] == 1);
		assert(s[1] == 2);		
	}

	{
		int* p = nullptr;
		auto s = gsl::make_span(p, gsl::narrow_cast<gsl::span<int>::size_type>(0));
		assert(s.size() == 0);
		assert(s.data() == nullptr);
	}

	{
		int* p = nullptr;
		auto workaround_macro = [=]() {gsl::make_span(p, 2); };
	}

	// from pointer pointer ctor
	{
		auto s = gsl::make_span(&arr[0], &arr[2]);
		assert(s.size() == 2);
		assert(s.data() == &arr[0]);
		assert(s[0] == 1);
		assert(s[1] == 2);
	}

	{
		auto s = gsl::make_span(arr);
		assert(s.size() == 5);
		assert(s.data() == std::addressof(arr[0]));
	}

	std::cout << "span_ext_main successful\n";
	return 0;
}