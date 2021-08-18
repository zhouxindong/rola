#include <iostream>
#include "pointers.hpp"

#include <algorithm> // for addressof
#include <memory>    // for shared_ptr, make_shared, operator<, opera...
#include <sstream>   // for operator<<, ostringstream, basic_ostream:...
#include <stdint.h>  // for uint16_t
#include <string>    // for basic_string, operator==, string, operator<<
#include <typeinfo>  // for type_info
#include <vector>
#include <cassert>

struct MyBase
{
};
struct MyDerived : public MyBase
{
};
struct Unrelated
{
};

// stand-in for a user-defined ref-counted class
template <typename T>
struct RefCounted
{
	RefCounted(T* p) : p_(p) {}
	operator T* () { return p_; }
	T* p_;
};

// user defined smart pointer with comparison operators returning non bool value
template <typename T>
struct CustomPtr
{
	CustomPtr(T* p) : p_(p) {}
	operator T* () { return p_; }
	bool operator!=(std::nullptr_t) const { return p_ != nullptr; }
	T* p_ = nullptr;
};

template <typename T, typename U>
std::string operator==(CustomPtr<T> const& lhs, CustomPtr<U> const& rhs)
{
	GSL_SUPPRESS(type.1) // NO-FORMAT: attribute
		return reinterpret_cast<const void*>(lhs.p_) == reinterpret_cast<const void*>(rhs.p_) ? "true"
		: "false";
}

template <typename T, typename U>
std::string operator!=(CustomPtr<T> const& lhs, CustomPtr<U> const& rhs)
{
	GSL_SUPPRESS(type.1) // NO-FORMAT: attribute
		return reinterpret_cast<const void*>(lhs.p_) != reinterpret_cast<const void*>(rhs.p_) ? "true"
		: "false";
}

template <typename T, typename U>
std::string operator<(CustomPtr<T> const& lhs, CustomPtr<U> const& rhs)
{
	GSL_SUPPRESS(type.1) // NO-FORMAT: attribute
		return reinterpret_cast<const void*>(lhs.p_) < reinterpret_cast<const void*>(rhs.p_) ? "true"
		: "false";
}

template <typename T, typename U>
std::string operator>(CustomPtr<T> const& lhs, CustomPtr<U> const& rhs)
{
	GSL_SUPPRESS(type.1) // NO-FORMAT: attribute
		return reinterpret_cast<const void*>(lhs.p_) > reinterpret_cast<const void*>(rhs.p_) ? "true"
		: "false";
}

template <typename T, typename U>
std::string operator<=(CustomPtr<T> const& lhs, CustomPtr<U> const& rhs)
{
	GSL_SUPPRESS(type.1) // NO-FORMAT: attribute
		return reinterpret_cast<const void*>(lhs.p_) <= reinterpret_cast<const void*>(rhs.p_) ? "true"
		: "false";
}

template <typename T, typename U>
std::string operator>=(CustomPtr<T> const& lhs, CustomPtr<U> const& rhs)
{
	GSL_SUPPRESS(type.1) // NO-FORMAT: attribute
		return reinterpret_cast<const void*>(lhs.p_) >= reinterpret_cast<const void*>(rhs.p_) ? "true"
		: "false";
}

struct NonCopyableNonMovable
{
	NonCopyableNonMovable() = default;
	NonCopyableNonMovable(const NonCopyableNonMovable&) = delete;
	NonCopyableNonMovable& operator=(const NonCopyableNonMovable&) = delete;
	NonCopyableNonMovable(NonCopyableNonMovable&&) = delete;
	NonCopyableNonMovable& operator=(NonCopyableNonMovable&&) = delete;
};

GSL_SUPPRESS(f.4) // NO-FORMAT: attribute
bool helper(gsl::not_null<int*> p) { return *p == 12; }
GSL_SUPPRESS(f.4) // NO-FORMAT: attribute
bool helper_const(gsl::not_null<const int*> p) { return *p == 12; }

int* return_pointer() { return nullptr; }

template <typename T>
void ostream_helper(T v)
{
	gsl::not_null<T*> p(&v);
	{
		std::ostringstream os;
		std::ostringstream ref;
		os << static_cast<void*>(p);
		ref << static_cast<void*>(&v);
		assert(os.str() == ref.str());
	}
	{
		std::ostringstream os;
		std::ostringstream ref;
		os << *p;
		ref << v;
		assert(os.str() == ref.str());
	}
}

void f(int* i)
{
	*i += 1;
}

int main_pointers()
{
	static_assert(!gsl::details::is_comparable_to_nullptr<int>::value);
	static_assert(gsl::details::is_comparable_to_nullptr<int*>::value);
	static_assert(gsl::details::is_comparable_to_nullptr<decltype(nullptr)>::value);

	// not_null<T> constructors
	{
		//gsl::not_null<int*> p = nullptr; // compile error
		//gsl::not_null<std::vector<char>*> p1 = 0;	// compile error
		//gsl::not_null<int*> p2;	// compile error

		std::unique_ptr<int> up = std::make_unique<int>(120);
		gsl::not_null<int*> p3 = up;

		//gsl::not_null<std::vector<int>> f(std::vector<int>{1});
		//gsl::not_null<int> z(10);
		//gsl::not_null<std::vector<int>> y({ 1,2 });
	}

	// from shared pointer
	{
		int i = 12;
		auto rp = RefCounted<int>(&i);
		gsl::not_null<int*> p(rp);
		assert(p.get() == &i);

		gsl::not_null<std::shared_ptr<int>> x(std::make_shared<int>(10));
		assert(*(x.get()) == 10);
	}

	// from pointer to local
	{
		int t = 42;
		gsl::not_null<int*> x = &t;
		helper(&t);
		helper_const(&t);
		assert(*x == 42);
	}

	// from raw pointer
	// from not_null pointer
	{
		int t = 42;
		int* p = &t;

		gsl::not_null<int*> x = p;
		helper(p);
		helper_const(p);
		helper(x);
		helper_const(x);

		assert(*x == 42);
	}

	// from raw const pointer
	// from not_null const pointer
	{
		int t = 42;
		const int* cp = &t;
		gsl::not_null<const int*> x = cp;
		helper_const(cp);
		helper_const(x);
		assert(*x == 42);
	}

	// from not_null const pointer, using auto
	{
		int t = 42;
		const int* cp = &t;
		auto x = gsl::not_null<const int*>{ cp };
		assert(*x == 42);
	}

	// not_null ostream
	{
		ostream_helper<int>(17);
		ostream_helper<float>(21.5f);
		ostream_helper<double>(3.4566e-7);
		ostream_helper<char>('c');
		ostream_helper<uint16_t>(0x0123u);
		ostream_helper<const char*>("c string");
		ostream_helper<std::string>("string");
	}

	// not_null casting
	{
		MyBase base;
		MyDerived derived;
		Unrelated unrelated;

		gsl::not_null<Unrelated*> u{ &unrelated };
		(void)u;
		gsl::not_null<MyDerived*> p{ &derived };
		gsl::not_null<MyBase*> q(&base);
		q = p;
		assert(q == p);

		//q = u; 
		//p = q;

		//gsl::not_null<Unrelated*> r = p;
		//gsl::not_null<Unrelated*> s = reinterpret_cast<Unrelated*>(p);

		gsl::not_null<Unrelated*> t(reinterpret_cast<Unrelated*>(p.get()));
		assert(reinterpret_cast<void*>(p.get()) == reinterpret_cast<void*>(t.get()));
	}

	// not_null assignment
	{
		int i = 12;
		gsl::not_null<int*> p(&i);
		assert(helper(p));
	}

	// not_null comparison
	{
		int ints[2] = { 42,43 };
		int* p1 = &ints[0];
		const int* p2 = &ints[1];

		using NotNull1 = gsl::not_null<decltype(p1)>;
		using NotNull2 = gsl::not_null<decltype(p2)>;

		assert((NotNull1(p1) == NotNull1(p1)));
		assert(NotNull2(p2) == NotNull2(p2));
	}

	// owner
	{
		gsl::owner<int*> p = new int(120);
		assert(*p == 120);
		f(p);
		assert(*p == 121);
		delete p;

		//gsl::owner<int> integerTest = 10;	// compile error
		//gsl::owner<std::shared_ptr<int>> sharedPtrTest(new int(10));	// compile error
	}

	std::cout << "pointers_main successful\n";
	return 0;
}