#ifndef ROLA_LINGEX_CONCATENATION_HPP
#define ROLA_LINGEX_CONCATENATION_HPP

#include <initializer_list>

namespace rola
{
	template <typename T, typename ... Ts>
	auto concat_callable(T t, Ts ... ts)
	{
		if constexpr (sizeof...(ts) > 0)
		{
			return [=](auto ... parameters)
			{
				return t(concat_callable(ts...)(parameters...));
			};
		}
		else
			return t;
	}

	template <typename A, typename B, typename F>
	auto combine(F binary_func, A a, B b)
	{
		return [=](auto param)
		{
			return binary_func(a(param), b(param));
		};
	}

	template <typename ... Calls>
	auto multi_call(Calls ... call)
	{
		return [=](auto ... x) {
			(void)std::initializer_list<int>{
				((void)call(x...), 0)...
			};
		};
	}

	template <typename F, typename ... Args>
	auto call_each(F f, Args ... args)
	{
		(void)std::initializer_list<int>{
			((void)f(args), 0)...
		};
	}
} // namespace rola

#endif // !ROLA_LINGEX_CONCATENATION_HPP
