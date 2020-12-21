#include "easy_bind.hpp"
#include <type_traits>
#include <cassert>

#include <iostream>
int main_ebm()
{
	assert((std::is_base_of_v<
		rola::detail::Indices<0, 1, 2>,
		rola::detail::Build_indices<3>> == true));

	std::cout << "easy_bind test successful\n";

	return 0;
}