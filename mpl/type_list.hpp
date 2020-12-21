#ifndef ROLA_MPL_TYPE_LIST_HPP
#define ROLA_MPL_TYPE_LIST_HPP

#include <type_traits>

namespace rola
{
	// primary class
	template <typename ...Elements>
	class Type_list {};

	namespace TL
	{
		// meta functions

#pragma region Length
		
		template <typename List>
		class Length;

		template <typename Head, typename ...Tail>
		class Length<Type_list<Head, Tail...>>
		{
		public:
			static constexpr int value = 1 + Length<Type_list<Tail...>>::value;
		};

		template <>
		class Length<Type_list<>>
		{
		public:
			static constexpr int value = 0;
		};

		template <typename List>
		inline constexpr int Length_v = Length<List>::value;

#pragma endregion

#pragma region Front

		template <typename List>
		class Front;

		template <typename Head, typename ...Tail>
		class Front<Type_list<Head, Tail...>>
		{
		public:
			using type = Head;
		};

		template <>
		class Front<Type_list<>>
		{
		public:
			using type = void;
		};

		template <typename List>
		using Front_t = typename Front<List>::type;

#pragma endregion

#pragma region Pop_front

		template <typename List>
		class Pop_front;

		template <typename Head, typename ...Tail>
		class Pop_front<Type_list<Head, Tail...>>
		{
		public:
			using type = Type_list<Tail...>;
		};

		template <typename List>
		using Pop_front_t = typename Pop_front<List>::type;

#pragma endregion

#pragma region Push_front

		template <typename List, typename NewElement>
		class Push_front;

		template <typename ...Elements, typename NewElement>
		class Push_front<Type_list<Elements...>, NewElement>
		{
		public:
			using type = Type_list<NewElement, Elements...>;
		};

		template <typename List, typename NewElement>
		using Push_front_t = typename Push_front<List, NewElement>::type;

#pragma endregion

#pragma region N_element

		template <typename List, unsigned N>
		class N_element;

		template <typename Head, typename ...Tail, unsigned N>
		class N_element<Type_list<Head, Tail...>, N> : public N_element<Type_list<Tail...>, N-1>
		{
		};

		template <typename Head, typename ...Tail>
		class N_element<Type_list<Head, Tail...>, 0>
		{
		public:
			using type = Head;
		};

		template <unsigned N>
		class N_element<Type_list<>, N>
		{
		public:
			using type = void;
		};

		// another implement
		//template <typename List, unsigned N>
		//class N_element : public N_element<Pop_front_t<List>, N - 1>
		//{
		//};

		//template <typename List>
		//class N_element<List, 0> : public Front<List>
		//{
		//};

		template <typename List, unsigned N>
		using N_element_t = typename N_element<List, N>::type;
#pragma endregion

#pragma region Index_of

		template <typename List, typename Element>
		class Index_of {};

		template <typename Head, typename ...Tail, typename Element>
		class Index_of<Type_list<Head, Tail...>, Element>
		{
			static constexpr int r_value = Index_of<Type_list<Tail...>, Element>::value;
		public:
			static constexpr int value = r_value == -1 ? -1 : 1 + r_value;
		};

		template <typename ...Tail, typename Element>
		class Index_of<Type_list<Element, Tail...>, Element>
		{
		public:
			static constexpr int value = 0;
		};

		template <typename Element>
		class Index_of<Type_list<>, Element>
		{
		public:
			static constexpr int value = -1;
		};

		template <typename List, typename Element>
		inline constexpr int Index_of_v = Index_of<List, Element>::value;

#pragma endregion

#pragma region Largest_type

		template <typename List>
		class Largest_type;

		template <typename Head, typename ...Tail>
		class Largest_type<Type_list<Head, Tail...>>
		{
			using Rest = typename Largest_type<Type_list<Tail...>>::type;

		public:
			using type = std::conditional_t<(sizeof(Rest) > sizeof(Head)), Rest, Head>;
		};

		template <>
		class Largest_type<Type_list<>>
		{
		public:
			using type = char;
		};

		template <typename List>
		using Largest_type_t = typename Largest_type<List>::type;
#pragma endregion

#pragma region Is_empty

		template <typename List>
		class Is_empty;

		template <typename ...Elements>
		class Is_empty<Type_list<Elements...>>
		{
		public:
			static constexpr bool value = false;
		};

		template <>
		class Is_empty<Type_list<>>
		{
		public:
			static constexpr bool value = true;
		};

		template <typename List>
		inline constexpr bool Is_empty_v = Is_empty<List>::value;
#pragma endregion

#pragma region Push_back

		template <typename List, typename NewElement>
		class Push_back;

		template <typename ...Elements, typename NewElement>
		class Push_back<Type_list<Elements...>, NewElement>
		{
		public:
			using type = Type_list<Elements..., NewElement>;
		};

		template <typename List, typename NewElement>
		using Push_back_t = typename Push_back<List, NewElement>::type;
#pragma endregion

#pragma region Back

		template <typename List>
		class Back;

		template <typename Head, typename ...Tail>
		class Back<Type_list<Head, Tail...>> : public Back<Type_list<Tail...>>
		{
		};

		template <typename T>
		class Back<Type_list<T>>
		{
		public:
			using type = T;
		};

		template <>
		class Back<Type_list<>>
		{
		public:
			using type = void;
		};

		template <typename List>
		using Back_t = typename Back<List>::type;
#pragma endregion

#pragma region Pop_back

		template <typename List>
		class Pop_back;

		template <typename Head, typename ...Tail>
		class Pop_back<Type_list<Head, Tail...>>
		{
		public:
			using type = Push_front_t<typename Pop_back<Type_list<Tail...>>::type,
				Head>;
		};

		template <typename T>
		class Pop_back<Type_list<T>>
		{
		public:
			using type = Type_list<>;
		};

		template <>
		class Pop_back<Type_list<>>
		{
		public:
			using type = Type_list<>;
		};

		template <typename List>
		using Pop_back_t = typename Pop_back<List>::type;
#pragma endregion

#pragma region Erase

		template <typename List, typename Element>
		class Erase;

		template <typename Head, typename ...Tail, typename Element>
		class Erase<Type_list<Head, Tail...>, Element>
		{
		public:
			using type = Push_front_t<typename Erase<Type_list<Tail...>, Element>::type, Head>;
		};

		template <typename Head, typename ...Tail>
		class Erase<Type_list<Head, Tail...>, Head>
		{
		public:
			using type = Type_list<Tail...>;
		};

		template <typename Element>
		class Erase<Type_list<>, Element>
		{
		public:
			using type = Type_list<>;
		};

		template <typename List, typename Element>
		using Erase_t = typename Erase<List, Element>::type;

#pragma endregion

#pragma region Erase_all

		template <typename List, typename Element>
		class Erase_all;

		template <typename Element>
		class Erase_all<Type_list<>, Element>
		{
		public:
			using type = Type_list<>;
		};

		template <typename Head, typename ...Tail>
		class Erase_all<Type_list<Head, Tail...>, Head>
		{
		public:
			using type = typename Erase_all<Type_list<Tail...>, Head>::type;
		};

		template <typename Head, typename ...Tail, typename Element>
		class Erase_all<Type_list<Head, Tail...>, Element>
		{
		public:
			using type = Push_front_t<typename Erase_all<Type_list<Tail...>, Element>::type, Head>;
		};

		template <typename List, typename Element>
		using Erase_all_t = typename Erase_all<List, Element>::type;

#pragma endregion

#pragma region Unique

		template <typename List>
		class Unique;

		template <>
		class Unique<Type_list<>>
		{
		public:
			using type = Type_list<>;
		};

		template <typename Head, typename ...Tail>
		class Unique<Type_list<Head, Tail...>>
		{
			using L1 = typename Unique<Type_list<Tail...>>::type;
			using L2 = typename Erase<L1, Head>::type;
		public:
			using type = Push_front_t<L2, Head>;
		};

		template <typename List>
		using Unique_t = typename Unique<List>::type;

#pragma endregion

#pragma region Replace

		template <typename List, typename From, typename To>
		class Replace;

		template <typename Head, typename ...Tail, typename From, typename To>
		class Replace<Type_list<Head, Tail...>, From, To>
		{
			using L1 = typename Replace<Type_list<Tail...>, From, To>::type;
		public:
			using type = Push_front_t<L1, Head>;
		};

		template <typename ...Tail, typename From, typename To>
		class Replace<Type_list<From, Tail...>, From, To>
		{
		public:
			using type = Type_list<To, Tail...>;
		};

		template <typename From, typename To>
		class Replace<Type_list<>, From, To>
		{
		public:
			using type = Type_list<>;
		};

		template <typename List, typename From, typename To>
		using Replace_t = typename Replace<List, From, To>::type;
#pragma endregion

#pragma region Replace_all

		template <typename List, typename From, typename To>
		class Replace_all;

		template <typename Head, typename ...Tail, typename From, typename To>
		class Replace_all<Type_list<Head, Tail...>, From, To>
		{
			using L1 = typename Replace_all<Type_list<Tail...>, From, To>::type;

		public:
			using type = Push_front_t<L1, Head>;
		};

		template <typename ...Tail, typename From, typename To>
		class Replace_all<Type_list<From, Tail...>, From, To>
		{
		public:
			using type = Push_front_t<typename Replace_all<Type_list<Tail...>, From, To>::type, To>;
		};

		template <typename From, typename To>
		class Replace_all<Type_list<>, From, To>
		{
		public:
			using type = Type_list<>;
		};

		template <typename List, typename From, typename To>
		using Replace_all_t = typename Replace_all<List, From, To>::type;

#pragma endregion

#pragma region Reverse

		template <typename List>
		class Reverse;

		template <typename Head, typename ...Tail>
		class Reverse<Type_list<Head, Tail...>>
		{
		public:
			using type = Push_back_t<typename Reverse<Type_list<Tail...>>::type, Head>;
		};

		template <>
		class Reverse<Type_list<>>
		{
		public:
			using type = Type_list<>;
		};

		template <typename List>
		using Reverse_t = typename Reverse<List>::type;

#pragma endregion

#pragma region Most_derived

		template <typename List, typename Base>
		class Most_derived;

		template <typename Base>
		class Most_derived<Type_list<>, Base>
		{
		public:
			using type = Base;
		};

		template <typename Head, typename ...Tail, typename Base>
		class Most_derived<Type_list<Head, Tail...>, Base>
		{
			using L = typename Most_derived<Type_list<Tail...>, Base>::type;

		public:
			using type = std::conditional_t<
				std::is_base_of_v<L, Head>,
				Head,
				L>;
		};

		template <typename List, typename Base>
		using Most_derived_t = typename Most_derived<List, Base>::type;

#pragma endregion

#pragma region Derived_to_front

		template <typename List>
		class Derived_to_front;

		template <>
		class Derived_to_front<Type_list<>>
		{
		public:
			using type = Type_list<>;
		};

		template <typename Head, typename ...Tail>
		class Derived_to_front<Type_list<Head, Tail...>>
		{
			using MD = Most_derived_t<Type_list<Tail...>, Head>;
			using L = Replace_t<Type_list<Tail...>, MD, Head>;

		public:
			using type = Push_front_t<L, MD>;
		};

		template <typename List>
		using Derived_to_front_t = typename Derived_to_front<List>::type;

#pragma endregion

		// map-reduce-filter
#pragma region transform (map)

		template <typename List, template <typename> class MetaFun, bool Empty = Is_empty_v<List>>
		class Transform;

		template <typename ...Elements, template <typename> class MetaFun>
		class Transform<Type_list<Elements...>, MetaFun, false>
		{
		public:
			using type = Type_list<typename MetaFun<Elements>::type...>;
		};

		template <template <typename> class MetaFun>
		class Transform<Type_list<>, MetaFun, true>
		{
		public:
			using type = Type_list<>;
		};

		template <typename List, template <typename> class MetaFun, bool Empty = Is_empty_v<List>>
		using Transform_t = typename Transform<List, MetaFun>::type;
#pragma endregion

#pragma region accumulate (reduce)

		template <
			typename List,
			template <typename, typename> class F,
			typename I>
			class Accumulate;

		template <
			typename Head, typename ...Tail,
			template <typename, typename> class F,
			typename I>
			class Accumulate<Type_list<Head, Tail...>, F, I>
		{
		public:
			using type = typename Accumulate<Type_list<Tail...>, F, typename F<I, Head>::type>::type;
		};

		template <
			template <typename, typename> class F,
			typename I>
			class Accumulate<Type_list<>, F, I>
		{
		public:
			using type = I;
		};

		template <
			typename List,
			template <typename, typename> class F,
			typename I>
			using Accumulate_t = typename Accumulate<List, F, I>::type;
#pragma endregion

#pragma region filter

		template <
			typename List,
			template <typename> class Predicate>
		class Filter;

		template <
			template <typename> class Predicate>
		class Filter<Type_list<>, Predicate>
		{
		public:
			using type = Type_list<>;
		};

		template <
			typename Head, typename ...Tail,
			template <typename> class Predicate>
		class Filter<Type_list<Head, Tail...>, Predicate>
		{
			using T1 = typename Filter<Type_list<Tail...>, Predicate>::type;

		public:
			using type = std::conditional_t<
				Predicate<Head>::value,
				Push_front_t<T1, Head>,
				T1>;
		};

		template <typename List, template <typename> class Predicate>
		using Filter_t = typename Filter<List, Predicate>::type;


#pragma endregion

		// apply
#pragma region apply
		template <typename List, template <typename> class F>
		class Apply;

		template <typename T, template <typename> class F>
		class Apply : public F<T>
		{
		};

		template <typename ...Elements, template <typename> class F>
		class Apply<Type_list<Elements...>, F>
			: public Apply<Elements, F>...
		{
		};

		template <template <typename> class F>
		class Apply<Type_list<>, F>
		{
		};

		// 1. cast
		template <typename T, typename List,
			template <typename> class F>
		F<T>& field(Apply<List, F>& obj)
		{
			return obj;
		}

		template <typename T, typename List,
			template <typename> class F>
		F<T> const& field(Apply<List, F> const& obj)
		{
			return obj;
		}

#pragma endregion
	} // namespace TL
} // namespace rola

#endif // !ROLA_MPL_TYPE_LIST_HPP
