#include "value_list.hpp"

using T0 = rola::Value_list<int>;
using T1 = rola::Value_list<int, 1>;
using T2 = rola::Value_list<int, 1, 2>;
using T3 = rola::Value_list<int, 1, 2, 3>;

int main_vlm()
{
	static_assert(rola::TL::Is_empty_v<rola::Value_list<int>>);
	static_assert(!rola::TL::Is_empty_v<rola::Value_list<int, 1>>);

	static_assert(std::is_same_v<
		rola::CT_value<int, 1>,
		rola::TL::Front_t<T1>
	>);

	static_assert(std::is_same_v<
		void,
		rola::TL::Front_t<T0>
	>);

	static_assert(std::is_same_v<
		T0,
		rola::TL::Pop_front_t<T0>
	>);
	static_assert(std::is_same_v<
		T0,
		rola::TL::Pop_front_t<T1>
	>);
	static_assert(std::is_same_v<
		rola::Value_list<int, 2>,
		rola::TL::Pop_front_t<T2>
	>);
	static_assert(std::is_same_v<
		rola::Value_list<int, 2, 3>,
		rola::TL::Pop_front_t<T3>
	>);

	static_assert(std::is_same_v<
		T1,
		rola::TL::Push_front_t<T0, rola::CT_value<int, 1>>
	>);
	static_assert(std::is_same_v <
		rola::TL::Push_front_t<T1, rola::CT_value<int, 99>>,
		rola::Value_list<int, 99, 1>
	>);
	static_assert(std::is_same_v<
		rola::TL::Push_front_t<T3, rola::CT_value<int, -1>>,
		rola::Value_list<int, -1, 1, 2, 3>
	>);

	static_assert(std::is_same_v<
		rola::TL::Push_back_t<T0, rola::CT_value<int, 0>>,
		rola::Value_list<int, 0>
	>);
	static_assert(std::is_same_v<
		rola::TL::Push_back_t<T3, rola::CT_value<int, 3>>,
		rola::Value_list<int, 1, 2, 3, 3>
	>);

	static_assert(std::is_same_v<
		rola::TL::Reverse_t<T0>,
		T0
	>);

	static_assert(std::is_same_v<
		rola::TL::Reverse_t<T1>,
		T1
	>);

	static_assert(std::is_same_v<
		rola::TL::Reverse_t<T2>,
		rola::Value_list<int, 2, 1>
	>);

	static_assert(std::is_same_v<
		rola::TL::Reverse_t<T3>,
		rola::Value_list<int, 3, 2, 1>
	>);

	static_assert(std::is_same_v<
		rola::Value_list<unsigned, 0, 1, 2, 3, 4>,
		rola::TL::Reverse_t<rola::Value_list<unsigned, 4, 3, 2, 1, 0>>
	>);

	using My_index_list = rola::TL::Make_index_list_t<5>;
	static_assert(std::is_same_v<
		My_index_list,
		rola::Value_list<unsigned, 0, 1, 2, 3, 4>
	>);

	static_assert(std::is_same_v<
		rola::TL::Reverse_t<My_index_list>,
		rola::Value_list<unsigned, 4, 3, 2, 1, 0>
	>);
	return 0;
}