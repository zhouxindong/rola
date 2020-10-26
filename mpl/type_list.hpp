#ifndef ROLA_MPL_TYPE_LIST_HPP
#define ROLA_MPL_TYPE_LIST_HPP

namespace rola
{
	// primary class
	template <typename ...Elements>
	class Type_list {};

	namespace TL
	{
#pragma region Length
		
		template <typename>
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

		template <typename T>
		inline constexpr int Length_v = Length<T>::value;

#pragma endregion
	} // namespace TL
} // namespace rola

#endif // !ROLA_MPL_TYPE_LIST_HPP
