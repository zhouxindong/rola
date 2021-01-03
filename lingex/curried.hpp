#ifndef ROLA_LINGEX_CURRIED_HPP
#define ROLA_LINGEX_CURRIED_HPP

#include <tuple>
#include <string>
#include <iostream>
#include <functional>
#include <type_traits>
#include <utility>

namespace rola
{
	template <typename F, typename... CapArgs>
	class Curried
	{
		F function_;
		std::tuple<CapArgs...> args_;

	public:
		Curried(F f, CapArgs... args)
			: function_(f)
			, args_(std::make_tuple(args...))
		{}

		Curried(F f, std::tuple<CapArgs...> args)
			: function_(f)
			, args_(args)
		{}

		template <typename... NewArgs>
		auto operator()(NewArgs... args) const
		{
			auto new_args = std::make_tuple(args...);
			auto all_args = std::tuple_cat(args_, std::move(new_args));

			if constexpr (std::is_invocable_v<F, CapArgs..., NewArgs...>)
			{
				return std::apply(function_, all_args);
			}
			else
			{
				return Curried<F, CapArgs..., NewArgs...>(function_, all_args);
			}
		}
	};

	// Needed for pre-C++17 compilers
	template <typename Function>
	Curried<Function> make_curried(Function&& f)
	{
		return Curried<Function>(std::forward<Function>(f));
	}
} // namespace rola

#endif // !ROLA_LINGEX_CURRIED_HPP
