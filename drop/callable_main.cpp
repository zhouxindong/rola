#include "callable.h"

#include <iostream>
#include <string>
#include <typeinfo>

struct Foo {
	Foo(int num) : num_(num) {}
	void print_add(int i) const { std::cout << "void Foo::print_add(int)\n"; }
	int num_;
};

void print_num(int i)
{
	std::cout << "void print_num(int)\n";
}

struct PrintNum {
	void operator()(int i) const
	{
		std::cout << "void PrintNum::operator()(int)\n";
	}
};

auto l = [](int i)->double {return i + 0.23; };

int main_cm()
{
	std::string s;
	static_assert(!detail::is_reference_wrapper_v<std::string>);
	static_assert(detail::is_reference_wrapper_v<decltype(std::ref(s))>);

#pragma region invoke

	invoke(print_num, -9);								// 1. function
	invoke([]() {std::cout << "void lambda()\n"; });	// 2. lambda
	Foo foo(31);
	invoke(&Foo::print_add, foo, 1);					// 3. member function pointer
	std::cout << invoke(&Foo::num_, foo) << "\n";		// 4. member data pointer
	invoke(PrintNum(), 18);								// 5. functor

#pragma endregion

#pragma region c11::result_of

	// 1. function
	static_assert(std::is_same_v<void, typename c11::result_of<decltype(&print_num)(int)>::type>);
	// 2. lambda
	static_assert(std::is_same_v<double, typename c11::result_of<decltype(l)(int)>::type>);
	// 3. functor
	static_assert(std::is_same_v<void, typename c11::result_of<PrintNum(int)>::type>);
	// 4.a member function pointer - object
	static_assert(std::is_same_v<void, typename c11::result_of<decltype(&Foo::print_add)(Foo, int)>::type>);
	// 4.b member function pointer - pointer
	static_assert(std::is_same_v<void, typename c11::result_of<decltype(&Foo::print_add)(Foo*, int)>::type>);
	// 4.c member function pointer - ref
	static_assert(std::is_same_v<void, typename c11::result_of<decltype(&Foo::print_add)(
		decltype(std::ref(foo)), int)>::type>);

	// 5.a member data pointer - object
	using pmd = typename c11::result_of<decltype(&Foo::num_)(decltype(foo))>::type;
	static_assert(std::is_same_v<int&&, pmd>);
	// 5.b member data pointer - pointer
	using pmd2 = typename c11::result_of<decltype(&Foo::num_)(decltype(&foo))>::type;
	static_assert(std::is_same_v<int&, pmd2>);
	// 5.c member data pointer - ref
	using pmd3 = typename c11::result_of<decltype(&Foo::num_)(decltype(std::ref(foo)))>::type;
	static_assert(std::is_same_v<int&, pmd3>);

#pragma endregion

#pragma region c14::result_of

	// 1. function
	static_assert(std::is_same_v<void, typename c14::result_of<void, decltype(&print_num), int>::type>);
	// 2. lambda
	static_assert(std::is_same_v<double, typename c14::result_of<void, decltype(l), int>::type>);
	// 3. functor
	static_assert(std::is_same_v<void, typename c14::result_of<void, PrintNum, int>::type>);
	// 4.a member function pointer - object
	static_assert(std::is_same_v<void, typename c14::result_of<void, decltype(&Foo::print_add), Foo, int>::type>);
	// 4.b member function pointer - pointer
	static_assert(std::is_same_v<void, typename c14::result_of<void, decltype(&Foo::print_add), Foo*, int>::type>);
	// 4.c member function pointer - ref
	static_assert(std::is_same_v<void, typename c14::result_of<void, decltype(&Foo::print_add), decltype(std::ref(foo)), int>::type>);

	// 5.a member data pointer - object
	using pmdd = typename c14::result_of<void, decltype(&Foo::num_), decltype(foo)>::type;
	static_assert(std::is_same_v<int&&, pmdd>);
	// 5.b member data pointer - pointer
	using pmdd2 = typename c14::result_of<void, decltype(&Foo::num_), decltype(&foo)>::type;
	static_assert(std::is_same_v<int&, pmdd2>);
	// 5.c member data pointer - ref
	using pmdd3 = typename c14::result_of<void, decltype(&Foo::num_), decltype(std::ref(foo))>::type;
	static_assert(std::is_same_v<int&, pmdd3>);

#pragma endregion

#pragma region c17::result_of

	// 1. function
	static_assert(std::is_same_v<void, typename c17::result_of<decltype(&print_num)(int)>::type>);
	// 2. lambda
	static_assert(std::is_same_v<double, typename c17::result_of<decltype(l)(int)>::type>);
	// 3. functor
	static_assert(std::is_same_v<void, typename c17::result_of<PrintNum(int)>::type>);
	// 4.a member function pointer - object
	static_assert(std::is_same_v<void, typename c17::result_of<decltype(&Foo::print_add)(Foo, int)>::type>);
	// 4.b member function pointer - pointer
	static_assert(std::is_same_v<void, typename c17::result_of<decltype(&Foo::print_add)(Foo*, int)>::type>);
	// 4.c member function pointer - ref
	static_assert(std::is_same_v<void, typename c17::result_of<decltype(&Foo::print_add)(
		decltype(std::ref(foo)), int)>::type>);

	// 5.a member data pointer - object
	using pmddd = typename c17::result_of<decltype(&Foo::num_)(decltype(foo))>::type;
	static_assert(std::is_same_v<int&&, pmddd>);
	// 5.b member data pointer - pointer
	using pmddd2 = typename c17::result_of<decltype(&Foo::num_)(decltype(&foo))>::type;
	static_assert(std::is_same_v<int&, pmddd2>);
	// 5.c member data pointer - ref
	using pmddd3 = typename c17::result_of<decltype(&Foo::num_)(decltype(std::ref(foo)))>::type;
	static_assert(std::is_same_v<int&, pmddd3>);

#pragma endregion

#pragma region invoke2

	invoke2(print_num, -9);								// 1. function
	invoke2([]() {std::cout << "void lambda()\n"; });	// 2. lambda
	invoke2(&Foo::print_add, foo, 1);					// 3. member function pointer
	std::cout << invoke2(&Foo::num_, foo) << "\n";		// 4. member data pointer
	invoke2(PrintNum(), 18);							// 5. functor

#pragma endregion

	return 0;
}