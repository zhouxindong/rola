#include <iostream>

#include "byte.hpp" // for byte
#include "util.hpp" // for narrow_cast, at
#include "span.hpp"     // for span, span_iterator, operator==, operator!=

#include <array>       // for array
#include <iostream>    // for ptrdiff_t
#include <iterator>    // for reverse_iterator, operator-, operator==
#include <memory>      // for unique_ptr, shared_ptr, make_unique, allo...
#include <regex>       // for match_results, sub_match, match_results<>...
#include <cstddef>     // for ptrdiff_t
#include <string>      // for string
#include <type_traits> // for integral_constant<>::value, is_default_co...
#include <vector>      // for vector
#include <utility>
#include <cassert>

// the string_view include and macro are used in the deduction guide verification
#if (defined(__cpp_deduction_guides) && (__cpp_deduction_guides >= 201611L))
#ifdef __has_include
#if __has_include(<string_view>)
#include <string_view>
#define HAS_STRING_VIEW
#endif // __has_include(<string_view>)
#endif // __has_include
#endif // (defined(__cpp_deduction_guides) && (__cpp_deduction_guides >= 201611L))

namespace
{
	static constexpr char deathstring[] = "Expected Death";

	struct BaseClass
	{
	};
	struct DerivedClass : BaseClass
	{
	};
	struct AddressOverloaded
	{
#if (__cplusplus > 201402L)
		[[maybe_unused]]
#endif
		AddressOverloaded
			operator&() const
		{
			return {};
		}
	};
} // namespace

int main_span()
{
	// constructors
	{
		gsl::span<int> s;
		assert(s.size() == 0);
		assert(s.data() == nullptr);

		gsl::span<const int> cs;
		assert(cs.size() == 0);
		assert(cs.data() == nullptr);
	}

	// constructor with extent
	{
		gsl::span<int, 0> s;
		assert(s.size() == 0);
		assert(s.data() == nullptr);

		gsl::span<const int, 0> cs;
		assert(cs.size() == 0);
		assert(cs.data() == nullptr);
	}

	// from pointer length constructor
	{
		int arr[4] = { 1,2,3,4 };
		{
			for (int i = 0; i < 4; ++i)
			{
				{
					gsl::span<int> s = { &arr[0], gsl::narrow_cast<std::size_t>(i) };
					assert(s.size() == gsl::narrow_cast<std::size_t>(i));
					assert(s.data() == &arr[0]);
					assert(s.empty() == (i == 0));
					for (int j = 0; j < i; ++j)
						assert(arr[j] == s[gsl::narrow_cast<std::size_t>(j)]);
				}
				{
					gsl::span<int> s = { &arr[i], 4 - gsl::narrow_cast<std::size_t>(i) };
					assert(s.size() == 4 - gsl::narrow_cast<std::size_t>(i));
					assert(s.data() == &arr[i]);
					assert(s.empty() == ((4 - i) == 0));

					for (int j = 0; j < 4 - i; ++j)
						assert(arr[j + i] == s[gsl::narrow_cast<std::size_t>(j)]);
				}
			}
		}

		{
			gsl::span<int, 2> s{ &arr[0], 2 };
			assert(s.size() == 2);
			assert(s.data() == &arr[0]);
			assert(s[0] == 1);
			assert(s[1] == 2);
		}
	}

	// from pointer pointer construction
	{
		int arr[4] = { 1,2,3,4 };

		{
			gsl::span<int> s(&arr[0], &arr[2]);
			assert(s.size() == 2);
			assert(s.data() == &arr[0]);
			assert(s[0] == 1);
			assert(s[1] == 2);
		}

		{
			gsl::span<int, 2> s(&arr[0], &arr[2]);
			assert(s.size() == 2);
			assert(s.data() == &arr[0]);
			assert(s[0] == 1);
			assert(s[1] == 2);
		}

		{
			gsl::span<int> s(&arr[0], &arr[0]);
			assert(s.size() == 0);
			assert(s.data() == &arr[0]);
		}

		{
			gsl::span<int, 0> s(&arr[0], &arr[0]);
			assert(s.size() == 0);
			assert(s.data() == &arr[0]);
		}
	}

	// from array constructor
	{
		int arr[5] = { 1,2,3,4,5 };
		{
			const gsl::span<int> s{ arr };
			assert(s.size() == 5);
			assert(s.data() == &arr[0]);
		}

		{
			const gsl::span<int, 5> s{ arr };
			assert(s.size() == 5);
			assert(s.data() == &arr[0]);
		}
	}

	// from std array constructor
	{
		std::array<int, 4> arr = { 1,2,3,4 };

		{
			gsl::span<int> s{ arr };
			assert(s.size() == arr.size());
			assert(s.data() == arr.data());

			gsl::span<const int> cs{ arr };
			assert(cs.size() == arr.size());
			assert(cs.data() == arr.data());
		}
	}

	// from container constructor
	{
		std::vector<int> v = { 1,2,3 };
		const std::vector<int> cv = v;
		{
			gsl::span<int> s{ v };
			assert(s.size() == v.size());
			assert(s.data() == v.data());

			gsl::span<const int> cs{ v };
			assert(cs.size() == v.size());
			assert(cs.data() == v.data());
		}
	}

	// copy move and assignment
	{
		gsl::span<int> s1;
		assert(s1.empty());

		int arr[] = { 3,4,5 };

		gsl::span<const int> s2 = arr;
		assert(s2.size() == 3);
		assert(s2.data() == &arr[0]);

		s2 = s1;
		assert(s2.empty());

		auto get_temp_span = [&]() -> gsl::span<int> {return { &arr[1], 2 }; };
		auto use_span = [&](gsl::span<const int> s) {
			assert(s.size() == 2);
			assert(s.data() == &arr[1]);
		};
		use_span(get_temp_span());

		s1 = get_temp_span();
		assert(s1.size() == 2);
		assert(s1.data() == &arr[1]);
	}

	// first
	{
		int arr[5] = { 1,2,3,4,5 };
		{
			gsl::span<int, 5> av = arr;
			assert(av.first<2>().size() == 2);
			assert(av.first(2).size() == 2);
		}

		{
			gsl::span<int, 5> av = arr;
			assert(av.first<0>().size() == 0);
			assert(av.first(0).size() == 0);
		}

		{
			gsl::span<int, 5> av = arr;
			assert(av.first<5>().size() == 5);
			assert(av.first(5).size() == 5);
		}

		{
			gsl::span<int> av;
			assert(av.first<0>().size() == 0);
			assert(av.first(0).size() == 0);
		}
	}

	// last
	{
		int arr[5] = { 1,2,3,4,5 };
		{
			gsl::span<int, 5> av = arr;
			assert(av.last<2>().size() == 2);
			assert(av.last(2).size() == 2);
		}

		{
			gsl::span<int, 5> av = arr;
			assert(av.last<0>().size() == 0);
			assert(av.last(0).size() == 0);
		}

		{
			gsl::span<int, 5> av = arr;
			assert(av.last<5>().size() == 5);
			assert(av.last(5).size() == 5);
		}

		{
			gsl::span<int> av;
			assert(av.last<0>().size() == 0);
			assert(av.last(0).size() == 0);
		}
	}

	// sub span
	{
		int arr[5] = { 1,2,3,4,5 };
		{
			gsl::span<int, 5> av = arr;
			assert((av.subspan<2, 2>().size()) == 2);
			assert(decltype(av.subspan<2, 2>())::extent == 2);
			assert(av.subspan(2, 2).size() == 2);
			assert(av.subspan(2, 3).size() == 3);
		}

		{
			gsl::span<int, 5> av = arr;
			assert((av.subspan<0, 0>().size() == 0));
			assert(decltype(av.subspan<0, 0>())::extent == 0);
			assert(av.subspan(0, 0).size() == 0);
		}

		{
			gsl::span<int, 5> av = arr;
			assert((av.subspan<0, 5>().size()) == 5);
			assert(decltype(av.subspan<0, 5>())::extent == 5);
			assert(av.subspan(0, 5).size() == 5);
		}

		{
			gsl::span<int, 5> av = arr;
			assert(av.subspan<1>().size() == 4);
			assert(decltype(av.subspan<1>())::extent == 4);
		}

		{
			gsl::span<int> av = arr;
			assert(av.subspan(0).size() == 5);
			assert(av.subspan(1).size() == 4);
			assert(av.subspan(4).size() == 1);
			assert(av.subspan(5).size() == 0);
			const auto av2 = av.subspan(1);
			for (std::size_t i = 0; i < 4; ++i)
				assert(av2[i] == static_cast<int>(i) + 2);
		}
	}

	// iterator default init
	{
		gsl::span<int>::iterator it1;
		gsl::span<int>::iterator it2;
		assert(it1 == it2);
	}

	// iterator comparisons
	{
		int a[] = { 1,2,3,4 };
		{
			gsl::span<int> s = a;
			gsl::span<int>::iterator it = s.begin();
			auto it2 = it + 1;

			assert(it == it);
			assert(it == s.begin());
			assert(s.begin() == it);

			assert(it != it2);
			assert(it2 != it);
			assert(it != s.end());
			assert(it2 != s.end());
			assert(s.end() != it);

			assert(it < it2);
			assert(it <= it2);
			assert(it2 <= s.end());
			assert(it < s.end());

			assert(it2 > it);
			assert(it2 >= it);
			assert(s.end() > it2);
			assert(s.end() >= it2);
		}
	}

	// begin end
	{
		{
			int a[] = { 1,2,3,4 };
			gsl::span<int> s = a;

			gsl::span<int>::iterator it = s.begin();
			gsl::span<int>::iterator it2 = std::begin(s);
			assert(it == it2);

			it = s.end();
			it2 = std::end(s);
			assert(it == it2);
		}

		{
			int a[] = { 1,2,3,4 };
			gsl::span<int> s = a;

			auto it = s.begin();
			auto first = it;
			assert(it == first);
			assert(*it == 1);

			auto beyond = s.end();
			assert(it != beyond);

			assert(beyond - first == 4);
			assert(first - first == 0);
			assert(beyond - beyond == 0);

			++it;
			assert(it - first == 1);
			assert(*it == 2);
			*it = 22;
			assert(*it == 22);
			assert(beyond - it == 3);

			it = first;
			assert(it == first);
			while (it != s.end())
			{
				*it = 5;
				++it;
			}

			assert(it == beyond);
			assert(it - beyond == 0);

			for (const auto& n : s)
			{
				assert(n == 5);
			}
		}
	}

	// rbegin rend
	{
		{
			int a[] = { 1,2,3,4 };
			gsl::span<int> s = a;

			auto it = s.rbegin();
			auto first = it;
			assert(it == first);
			assert(*it == 4);

			auto beyond = s.rend();
			assert(it != beyond);

			assert(beyond - first == 4);
			assert(first - first == 0);
			assert(beyond - beyond == 0);

			++it;
			assert(it - s.rbegin() == 1);
			assert(*it == 3);
			*it = 22;
			assert(*it == 22);
			assert(beyond - it == 3);

			it = first;
			assert(it == first);
			while (it != s.rend())
			{
				*it = 5;
				++it;
			}

			assert(it == beyond);
			assert(it - beyond == 0);

			for (const auto& n : s)
			{
				assert(n == 5);
			}
		}

		// as bytes
		{
			int a[] = { 1,2,3,4 };

			{
				const gsl::span<const int> s = a;
				assert(s.size() == 4);
				const gsl::span<const gsl::byte> bs = gsl::as_bytes(s);
				assert(static_cast<const void*>(bs.data()) == static_cast<const void*>(s.data()));
				assert(bs.size() == s.size_bytes());
			}
		}

		// as writable bytes
		{
			int a[] = { 1,2,3,4 };
			{
				gsl::span<int> s;
				const auto bs = gsl::as_writable_bytes(s);
				assert(bs.size() == s.size());
			}
		}

		// copy algorithm
		{
			std::array<int, 5> src{ 1,2,3,4,5 };
			std::array<int, 10> dst{};

			const gsl::span<int> src_span(src);
			const gsl::span<int> dst_span(dst);

			gsl::copy(src_span, dst_span);
			gsl::copy(src_span, dst_span.subspan(src_span.size()));
			for (std::size_t i = 0; i < src.size(); ++i)
			{
				assert(dst[i] == src[i]);
				assert(dst[i + src.size()] == src[i]);
			}
		}
	}
	std::cout << "span_main successful\n";
	return 0;
}
