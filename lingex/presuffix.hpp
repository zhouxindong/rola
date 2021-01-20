#ifndef ROLA_LINGEX_PRESUFFIX_HPP
#define ROLA_LINGEX_PRESUFFIX_HPP

#include <functional>

namespace rola
{
	template <typename T>
	class Call_proxy
	{
		T* p;
		std::function<void()> suffix;
	
	public:
		Call_proxy(T* pp)
			: p(pp)
		{}

		~Call_proxy()
		{
			if (suffix)
				suffix();
		}

		T* operator->()
		{
			return p;
		}

		template <typename F>
		void set_suffix(F&& f)
		{
			suffix = std::forward<F>(f);
		}
	};

	template <typename T>
	class Presuffix
	{
		T* p;
		std::function<void()> prefix;
		std::function<void()> suffix;

	public:
		Presuffix(T* pp)
			: p(pp)
		{}

		Call_proxy<T> operator->()
		{
			if (prefix)
				prefix();

			Call_proxy<T> cp(p);
			if (suffix)
				cp.set_suffix(suffix);
			return cp;
		}

		template <typename F>
		void set_prefix(F&& f)
		{
			prefix = std::forward<F>(f);
		}

		template <typename F>
		void set_suffix(F&& f)
		{
			suffix = std::forward<F>(f);
		}
	};

} // namespace rola

#endif // !ROLA_LINGEX_PRESUFFIX_HPP
