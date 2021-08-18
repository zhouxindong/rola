#ifndef ROLA_EASY_SIGNAL_EASY_BIND_HPP
#define ROLA_EASY_SIGNAL_EASY_BIND_HPP

#include <functional>

namespace rola
{
	// placeholder
	template <int I>
	struct Place_holder
	{
		static constexpr int value = I;
		using type = Place_holder<I>;
	};
} // namespace rola

namespace std
{
	template <int I>
	struct is_placeholder<rola::Place_holder<I>>
		: std::integral_constant<int, I> {};
} // namespace std

namespace rola
{
	namespace detail
	{
		/*
		Build_indices<N> -> Indices<0, 1, ... , N-1>
		*/

		// Indices
		template <std::size_t... Is>
		struct Indices {};

		// primary template
		template <std::size_t... Is>
		struct Build_indices;

		// recurse
		template <std::size_t N, std::size_t... Is>
		struct Build_indices<N, Is...>
			: Build_indices<N - 1, N - 1, Is...>
		{};

		// basis
		template <std::size_t... Is>
		struct Build_indices<0, Is...>
			: Indices<Is...>
		{};
	} // namespace detail
} // namespace rola

#endif // !ROLA_EASY_SIGNAL_EASY_BIND_HPP
