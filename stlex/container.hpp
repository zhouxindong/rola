#ifndef ROLA_STLEX_CONTAINER_HPP
#define ROLA_STLEX_CONTAINER_HPP

#include <type_traits>
#include <functional>
#include <algorithm>
#include <numeric> // std::accumulate
 
namespace rola
{
	namespace stlex
	{
#pragma region utils

		// erase-remove style for container
		template <typename Container>
		inline Container& erase_remove(
			Container& c,
			std::function<bool(std::add_lvalue_reference_t<typename Container::value_type>)> f)
		{
			c.erase(std::remove_if(c.begin(), c.end(), f), c.end());
			return c;
		}

		// replace new value for set element
		template <typename Container>
		inline bool replace_key(
			Container& c,
			const typename Container::key_type& old_key,
			const typename Container::key_type& new_key)
		{
			typename Container::iterator pos;
			pos = c.find(old_key);
			if (pos != c.end())
			{
				c.insert(typename Container::value_type(new_key, pos->second));
				c.erase(pos);
				return true;
			}
			else
				return false;
		}

#pragma endregion
		namespace detail
		{
			template <typename T>
			struct Call_arg
			{
				using type = std::conditional_t<std::is_scalar<T>::value, T, T const&>;
			};

			// transform | select | map
			template <				
				template <typename, typename> class C,
				typename F,
				typename T,
				typename R = std::result_of_t<F(typename Call_arg<T>::type)>>
				auto pipe_map(C<T, std::allocator<T>> const& c, F&& f)
			{				
				C<R, std::allocator<R>> v;
				v.resize(c.size());
				std::transform(c.cbegin(), c.cend(), v.begin(), std::forward<F>(f));
				return v;
			}

			// filter
			template <
				template <typename, typename> class C,
				typename F,
				typename T>
				auto pipe_filter(C<T, std::allocator<T>> const& c, F&& f)
			{
				C<T, std::allocator<T>> v;
				std::copy_if(c.cbegin(), c.cend(), std::back_inserter(v), std::forward<F>(f));
				return v;
			}

			template <typename T>
			struct Reduce_policy
			{
				using type = T;
				static constexpr T init_value = T();
			};

			// accumulate | sink | reduce
			template <
				template <typename, typename> class C,
				typename T,
				typename P = Reduce_policy<T>>
				auto reduce(C<T, std::allocator<T>> const& c, 
					std::function<typename P::type(typename P::type, typename Call_arg<T>::type)> f)
			{
				return std::accumulate(c.cbegin(), c.cend(), P::init_value, f);
			}

		} // namespace detail

#pragma region pipe

		// transform | map | select
		template <
			template <typename, typename> class C,
			typename F,
			typename T,
			typename R = std::result_of_t<F(typename detail::Call_arg<T>::type)>>
			auto operator|(C<T, std::allocator<T>> const& c, F&& f)
		{
			return detail::pipe_map(c, std::forward<F>(f));
		}

		// filter
		template <
			template <typename, typename> class C,
			typename F,
			typename T>
			auto operator>(C<T, std::allocator<T>> const& c, F&& f)
		{
			return detail::pipe_filter(c, std::forward<F>(f));
		}

		// reduce | accumulate | sink
		template <
			template <typename, typename> class C,
			typename T,
			typename P = detail::Reduce_policy<T>>
			auto operator>>(C<T, std::allocator<T>> const& c,
				std::function<typename P::type(typename P::type, typename detail::Call_arg<T>::type)> f)
		{
			return detail::reduce(c, f);
		}

#pragma endregion
	} // namespace stlex
} // namespace rola

#endif // !ROLA_STLEX_CONTAINER_HPP
