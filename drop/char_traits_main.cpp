#include "char_traits.hpp"

int main_char_traits()
{
	std::cout << "	string: "
		<< std::string{ "Foo Bar Baz" } << '\n'
		<< "lc_string: "
		<< lc_string{ "Foo Bar Baz" } << '\n'
		<< "ci_string: "
		<< ci_string{ "Foo Bar Baz" } << '\n';

	ci_string user_input{ "MaGiC PaSsWoRd!" };
	ci_string password{ "magic password!" };

	if (user_input == password)
	{
		std::cout << "Passwords match: \"" << user_input
			<< "\" == \"" << password << "\"\n";
	}
	std::cout << "char_traits_main.cpp successful\n";
	return 0;
}