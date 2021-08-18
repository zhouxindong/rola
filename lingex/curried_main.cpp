#include "curried.hpp"
#include <iostream>
#include <string>
#include <sstream>

std::string foo(int i, double d, std::string const& s)
{
	std::ostringstream oss;
	oss << "i=" << i;
	oss << ", d=" << d;
	oss << ", s=" << s;
	return oss.str();
}

auto lb = [](int i, double d, std::string const& s) 
{
	std::ostringstream oss;
	oss << "i=" << i;
	oss << ", d=" << d;
	oss << ", s=" << s;
	return oss.str();
};

class bar
{
public:
	std::string operator()(int i, double d, std::string const& s) const
	{
		std::ostringstream oss;
		oss << "i=" << i;
		oss << ", d=" << d;
		oss << ", s=" << s;
		return oss.str();
	}

	static std::string static_fun(int i, double d, std::string const& s)
	{
		std::ostringstream oss;
		oss << "i=" << i;
		oss << ", d=" << d;
		oss << ", s=" << s;
		return oss.str();
	}

	std::string fun(int i, double d, std::string const& s) const
	{
		std::ostringstream oss;
		oss << "i=" << i;
		oss << ", d=" << d;
		oss << ", s=" << s;
		return oss.str();
	}
};

class callable_test {
public:
	template <typename T1, typename T2, typename T3>
	auto operator() (T1 x, T2 y, T3 z) const
	{
		return x + y + z;
	}

	template <typename T1, typename T2>
	auto operator() (T1 x, T2 y) const
	{
		return x + y;
	}

	int sum(int x, int y) const
	{
		return x + y;
	}
};

int main_curried()
{
	auto foo_cd = rola::Curried(foo);
	std::cout << "foo_cd: " << foo_cd(1)(3.567)("hello") << std::endl;

	auto lambda_cd = rola::Curried(lb);
	std::cout << "lambda_cd: " << lambda_cd(2)(23.2983)("world") << std::endl;

	auto functor_cd = rola::Curried(bar());
	std::cout << "functor_cd: " << functor_cd(8)(0.233)("functor") << std::endl;

	auto static_mem_fun_cd = rola::Curried(bar::static_fun);
	std::cout << "static_mem_fun_cd: " << static_mem_fun_cd(12)(928.3)("static_mem_fun") << std::endl;
	
	auto mem_fun_cd = rola::Curried(&bar::fun);
	std::cout << "mem_fun_cd: " << mem_fun_cd(bar())(999)(9.23)("mem_fun") << std::endl;
	bar b;
	std::cout << "mem_fun_cd: " << mem_fun_cd(b)(999)(9.23)("mem_fun") << std::endl;
	std::cout << "mem_fun_cd: " << mem_fun_cd(&b)(999)(9.23)("mem_fun") << std::endl;
	std::cout << "mem_fun_cd: " << mem_fun_cd(std::move(b))(999)(9.23)("mem_fun") << std::endl;

	auto less_curried = rola::Curried(std::less<>());

	std::cout << less_curried(42, 1) << std::endl;

	auto greater_than_42 = less_curried(42);

	std::cout << greater_than_42(1.0) << std::endl;
	std::cout << greater_than_42(100.0) << std::endl;

	callable_test ct;

	auto ct_curried = rola::Curried(ct);

	std::cout << ct_curried(1)(2, 3) << std::endl;

	auto ct_curried_one = rola::Curried(ct, 1);

	std::cout << ct_curried_one(2, 3) << std::endl;

	auto mc = rola::Curried(&callable_test::sum);
	mc(ct)(1)(2);
	std::cout << "curried_main successful\n";
	return 0;
}