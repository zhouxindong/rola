#ifndef ROLA_MPL_TUPLE_HPP
#define ROLA_MPL_TUPLE_HPP

#include <type_traits>
#include <ostream>
#include "value_list.hpp"

namespace rola
{
	namespace mpl
	{
		// primary
		template <typename ...Types>
		class Tuple;

		// basis
		template <>
		class Tuple<> {};

		// recursive
		template <typename Head, typename ...Tail>
		class Tuple<Head, Tail...>
		{
		private:
			Head head_;
			Tuple<Tail...> tail_;

		public: // ctor
			Tuple() {}

			Tuple(Head const& head, Tuple<Tail...> const& tail)
				: head_(head), tail_(tail) {}

			Tuple(Head const& head, Tail const&... tail) // suport Tuple(17, 3.14, "Hello") ctor
				: head_(head), tail_(tail...) {}

			// the most general ctor: support move-construct and different convertabled type
			// CAUTION: this ctor can aborb any ctor call when others is you intend!
			// please to disable some when you want others can be adopted!
			template <typename VHead, typename ...VTail,
				typename = std::enable_if_t<sizeof...(VTail) == sizeof...(Tail)>>
				Tuple(VHead&& vhead, VTail&&... vtail)
				: head_(std::forward<VHead>(vhead))
				, tail_(std::forward<VTail>(vtail)...) {}

			// ctor from another tuple
			template <typename VHead, typename ...VTail,
				typename = std::enable_if_t<sizeof...(VTail) == sizeof...(Tail)>>
				Tuple(Tuple<VHead, VTail...> const& other)
				: head_(other.get_head())
				, tail_(other.get_tail()) {}

		public: // accessors
			Head& get_head() { return head_; }
			Head const& get_head() const { return head_; }
			Tuple<Tail...>& get_tail() { return tail_; }
			Tuple<Tail...> const& get_tail() const { return tail_; }

		public: // operator
			template <typename T, T Index>
			auto& operator[](CT_value<T, Index>);
		};

#pragma region Tuple_get(combine the partial special and auto deduce for function)

		template <unsigned N>
		struct Tuple_get
		{
			template <typename Head, typename ...Tail>
			static auto& apply(Tuple<Head, Tail...> const& t)
			{
				return Tuple_get<N - 1>::apply(t.get_tail());
			}
		};

		template <>
		struct Tuple_get<0>
		{
			template <typename Head, typename ...Tail>
			static Head const& apply(Tuple<Head, Tail...> const& t)
			{
				return t.get_head();
			}
		};

		template <unsigned N, typename ...Types>
		auto& get(Tuple<Types...> const& t)
		{
			return Tuple_get<N>::apply(t);
		}

#pragma endregion

		template<typename Head, typename ...Tail>
		template<typename T, T Index>
		inline auto& Tuple<Head, Tail...>::operator[](CT_value<T, Index>)
		{
			return get<Index>(*this);
		}

		template <typename ...Types>
		auto make_tuple(Types&&... elems)
		{
			return Tuple<std::decay_t<Types>...>(std::forward<Types>(elems)...);
		}

#pragma region operators

		inline bool operator==(Tuple<> const&, Tuple<> const&)
		{
			return true;
		}

		template <
			typename Head1, typename ...Tail1,
			typename Head2, typename ...Tail2,
			typename = std::enable_if_t<sizeof...(Tail1) == sizeof...(Tail2)>>
			inline bool operator==(
				Tuple<Head1, Tail1...> const& lhs,
				Tuple<Head2, Tail2...> const& rhs)
		{
			return lhs.get_head() == rhs.get_head() &&
				lhs.get_tail() == rhs.get_tail();
		}

		inline void print_tuple(std::ostream& out, Tuple<> const&, bool is_first = true)
		{
			out << (is_first ? '(' : ')');
		}

		template <typename Head, typename ...Tail>
		inline void print_tuple(std::ostream& out, Tuple<Head, Tail...> const& t, bool is_first = true)
		{
			out << (is_first ? "(" : ", ");
			out << t.get_head();
			print_tuple(out, t.get_tail(), false);
		}

		template <typename ...Types>
		inline std::ostream& operator<<(std::ostream& out, Tuple<Types...> const& t)
		{
			print_tuple(out, t);
			return out;
		}
#pragma endregion

	} // namespace mpl

	namespace TL
	{
#pragma region algorithms

		template <typename ...Types>
		class Is_empty<rola::mpl::Tuple<Types...>>
		{
		public:
			static constexpr bool value = (sizeof...(Types) == 0);
		};

		template <typename Head, typename ...Tail>
		class Front<rola::mpl::Tuple<Head, Tail...>>
		{
		public:
			using type = Head;
		};

		template <>
		class Front<rola::mpl::Tuple<>>
		{
		public:
			using type = void;
		};

		template <typename Head, typename ...Tail>
		class Pop_front<rola::mpl::Tuple<Head, Tail...>>
		{
		public:
			using type = rola::mpl::Tuple<Tail...>;
		};

		template <>
		class Pop_front<rola::mpl::Tuple<>>
		{
		public:
			using type = rola::mpl::Tuple<>;
		};

		template <typename Head, typename ...Tail>
		class Pop_back<rola::mpl::Tuple<Head, Tail...>>
		{
		public:
			using type = Push_front_t<typename Pop_back<rola::mpl::Tuple<Tail...>>::type, Head>;
		};

		template <>
		class Pop_back<rola::mpl::Tuple<>>
		{
		public:
			using type = rola::mpl::Tuple<>;
		};

		template <typename ...Types, typename Element>
		class Push_front<rola::mpl::Tuple<Types...>, Element>
		{
		public:
			using type = rola::mpl::Tuple<Element, Types...>;
		};

		template <typename ...Types, typename Element>
		class Push_back<rola::mpl::Tuple<Types...>, Element>
		{
		public:
			using type = rola::mpl::Tuple<Types..., Element>;
		};

		template <typename Head, typename ...Tail>
		class Reverse<rola::mpl::Tuple<Head, Tail...>>
		{
		public:
			using type = Push_back_t<typename Reverse<rola::mpl::Tuple<Tail...>>::type, Head>;
		};

		template <>
		class Reverse<rola::mpl::Tuple<>>
		{
		public:
			using type = rola::mpl::Tuple<>;
		};

#pragma endregion
	} // namespace TL

	namespace mpl
	{
		// both compile-time and run-time computation
		template <typename ...Types, typename V>
		inline rola::TL::Push_front_t<Tuple<Types...>, V>
			push_front(Tuple<Types...> const& tuple, V const& value)
		{
			return rola::TL::Push_front_t<Tuple<Types...>, V>(value, tuple);
		}

		// basis
		template <typename V>
		inline Tuple<V> push_back(Tuple<> const&, V const& value)
		{
			return Tuple<V>(value);
		}

		// recursive
		template <typename Head, typename ...Tail, typename V>
		inline Tuple<Head, Tail..., V> 
			push_back(Tuple<Head, Tail...> const& tuple, V const& value)
		{
			return Tuple<Head, Tail..., V>(tuple.get_head(),
				push_back(tuple.get_tail(), value));
		}

		template <typename ...Types>
		inline rola::TL::Pop_front_t<Tuple<Types...>> 
			pop_front(Tuple<Types...> const& tuple)
		{
			return tuple.get_tail();
		}

		inline Tuple<> reverse(Tuple<> const& t)
		{
			return t;
		}
		
		// the version of reverse need many copy
		template <typename Head, typename ...Tail>
		inline rola::TL::Reverse_t<Tuple<Head, Tail...>> 
			reverse(Tuple<Head, Tail...> const& t)
		{
			return push_back(reverse(t.get_tail()), t.get_head());
		}

		template <typename ...Types>
		inline rola::TL::Pop_back_t<Tuple<Types...>>
			pop_back(Tuple<Types...> const& tuple)
		{
			return reverse(pop_front(reverse(tuple)));
		}

		// another version of reverse, only reverse index
		template <typename ...Elements, unsigned ...Indices>
		auto reverse_impl(Tuple<Elements...> const& t, Value_list<unsigned, Indices...>)
		{
			return rola::mpl::make_tuple(rola::mpl::get<Indices>(t)...);
		}

		template <typename ...Elements>
		auto reverse2(Tuple<Elements...> const& t)
		{
			return reverse_impl(t, 
				rola::TL::Reverse_t<rola::TL::Make_index_list_t<sizeof...(Elements)>>());
		}

		template <typename ...Elements, unsigned ...Indices>
		auto select(Tuple<Elements...> const& t, Value_list<unsigned, Indices...>)
		{
			return rola::mpl::make_tuple(rola::mpl::get<Indices>(t)...);
		}

		// splat
		template <unsigned I, unsigned N, typename IndexList = Value_list<unsigned>>
		class Replicated_indexlist;

		template <unsigned I, unsigned N, unsigned ...Indices>
		class Replicated_indexlist<I, N, Value_list<unsigned, Indices...>>
			: public Replicated_indexlist<I, N - 1, Value_list<unsigned, Indices..., I>>
		{};

		template <unsigned I, unsigned ...Indices>
		class Replicated_indexlist<I, 0, Value_list<unsigned, Indices...>>
		{
		public:
			using type = Value_list<unsigned, Indices...>;
		};

		template <unsigned I, unsigned N>
		using Replicated_indexlist_t = typename Replicated_indexlist<I, N>::type;

		template <unsigned I, unsigned N, typename ...Elements>
		auto splat(Tuple<Elements...> const& t)
		{
			return select(t, Replicated_indexlist_t<I, N>());
		}

#pragma region apply

		// unpack a Tuple obj as callable object arguments
		template <typename F, typename ...Elements, unsigned ...Indices>
		auto apply_impl(F f, Tuple<Elements...> const& t,
			Value_list<unsigned, Indices...>)
			-> decltype(f(get<Indices>(t)...))
		{
			return f(get<Indices>(t)...);
		}

		template <typename F, typename ...Elements, 
			unsigned N = sizeof...(Elements)>
			auto apply(F f, Tuple<Elements...> const& t)
			-> decltype(apply_impl(f, t, rola::TL::Make_index_list_t<N>()))
		{
			return apply_impl(f, t, rola::TL::Make_index_list_t<N>());
		}

#pragma endregion
	} // namespace mpl
} // namespace rola

#endif // !ROLA_MPL_TUPLE_HPP
