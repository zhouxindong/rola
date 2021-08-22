#include <iostream>
#include "lambdas.h"

int main()
{
	const auto func = MakeOverloaded(
		[](int) {std::cout << "Int!\n"; },
		[](float) {std::cout << "Float!\n"; }
	);
	func(10);
	func(10.0f);

	const auto test = overloaded17{
		[](const int& i) {std::cout << "int: " << i << '\n'; },
		[](const float& f) {std::cout << "float: " << f << '\n'; },
		[](const std::string& s) {std::cout << "string: " << s << '\n'; }
	};
	test("10.0f");

	std::cout << "lambda main successfult\n";
	return 0;
}