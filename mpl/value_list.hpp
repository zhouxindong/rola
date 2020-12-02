#ifndef ROLA_MPL_VALUE_LIST_HPP
#define ROLA_MPL_VALUE_LIST_HPP

#include "type_list.hpp"

namespace rola
{
	template <typename T, T Value>
	struct CT_value
	{
		static constexpr T value = Value;
	};

	namespace VL
	{
#pragma region multiply

		template <typename T, typename U>
		struct Multiply;

		template <typename T, T Value1, T Value2>
		struct Multiply<CT_value<T, Value1>, CT_value<T, Value2>>
		{
		public:
			using type = CT_value<T, Value1* Value2>;
		};

		template <typename T, typename U>
		using Multiply_t = typename Multiply<T, U>::type;

#pragma endregion
	} // namespace VL

	template <typename T, T ...Values>
	struct Value_list {};

	namespace TL
	{
		template <typename T, T ...Values>
		struct Is_empty<Value_list<T, Values...>>
		{
			static constexpr bool value = sizeof...(Values) == 0;
		};

		template <typename T, T Head, T ...Tail>
		struct Front<Value_list<T, Head, Tail...>>
		{
			using type = CT_value<T, Head>;
			static constexpr T value = Head;
		};

		template <typename T>
		struct Front<Value_list<T>>
		{
			using type = void;
		};

		template <typename T, T Head, T ...Tail>
		struct Pop_front<Value_list<T, Head, Tail...>>
		{
			using type = Value_list<T, Tail...>;
		};

		template <typename T>
		struct Pop_front<Value_list<T>>
		{
			using type = Value_list<T>;
		};

		template <typename T, T ...Values, T New>
		struct Push_front<Value_list<T, Values...>, CT_value<T, New>>
		{
			using type = Value_list<T, New, Values...>;
		};

		template <typename T, T ...Values, T New>
		struct Push_back<Value_list<T, Values...>, CT_value<T, New>>
		{
			using type = Value_list<T, Values..., New>;
		};

		template <typename T, T Head, T ...Tail>
		struct Reverse<Value_list<T, Head, Tail...>>
		{
			using type = Push_back_t<typename Reverse<Value_list<T, Tail...>>::type,
				CT_value<T, Head>>;
		};

		template <typename T>
		struct Reverse<Value_list<T>>
		{
			using type = Value_list<T>;
		};
	} // namespace TL

	namespace TL
	{
#pragma region Select
		template <typename Types, typename Indices>
		class Select;

		template <typename Types, unsigned ...Indices>
		class Select<Types, Value_list<unsigned, Indices...>>
		{
		public:
			using type = Type_list<N_element_t<Types, Indices>...>;
		};

		template <typename Types, typename Indices>
		using Select_t = typename Select<Types, Indices>::type;
#pragma endregion

#pragma region Make_index_list
		
		template <unsigned N, typename Result = Value_list<unsigned>>
		struct Make_index_list : Make_index_list<N - 1, Push_front_t<Result, CT_value<unsigned, N - 1>>>
		{
		};

		template <typename Result>
		struct Make_index_list<0, Result>
		{
			using type = Result;
		};

		template <unsigned N>
		using Make_index_list_t = typename Make_index_list<N>::type;

#pragma endregion
	} // namespace TL
} // namespace rola

#endif // !ROLA_MPL_VALUE_LIST_HPP
