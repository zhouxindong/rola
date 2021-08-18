#ifndef ROLA_LINGEX_FUNCTIONIC_HPP
#define ROLA_LINGEX_FUNCTIONIC_HPP

namespace rola
{
	template <typename F, typename G>
	auto compose(F&& f, G&& g)
	{
		return [=](auto x)
		{
			return f(g(x));
		};
	}

	template <typename F, typename ... R>
	auto compose(F&& f, R&& ... r)
	{
		return [=](auto x)
		{
			return f(compose(r...)(x));
		};
	}

	namespace details
	{
		template <class F, class T, std::size_t ... I>
		auto apply(F&& f, T&& t, std::index_sequence<I ...>)
		{
			return std::invoke(
				std::forward<F>(f),
				std::get<I>(std::forward<T>(t))...);
		}
	}

	template <class F, class T>
	auto apply(F&& f, T&& t)
	{
		return details::apply(
			std::forward<F>(f),
			std::forward<T>(t),
			std::make_index_sequence<
			std::tuple_size<std::decay_t<T>>::value>{}
		);
	}

	template <typename ... Ts>
	struct overload_functor : Ts ...
	{
		using Ts::operator()...;
	};

	template <typename ... Ts>
	overload_functor(Ts...)->overload_functor<Ts...>;
} // namespace rola

#endif // !ROLA_LINGEX_FUNCTIONIC_HPP
