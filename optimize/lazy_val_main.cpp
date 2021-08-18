#include "lazy_val.hpp"
#include <iostream>

int main()
{
	auto lv = rola::make_lazy_val([] {return 99; });
	std::cout << lv << "\n";

    int number = 6;

    auto val = lazy{
        std::cout << "Calculating the answer..." << std::endl;
        std::cout << "while the number is: " << number << std::endl;
        return 42;
    };

    number = 2;

    std::cout << "Lazy value defined" << std::endl;

    std::cout << val << std::endl;
	return 0;
}