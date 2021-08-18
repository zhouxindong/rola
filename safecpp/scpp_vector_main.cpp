#include <iostream>
#include "scpp_vector.hpp"

int main_scpp_vector()
{
	rola::vector<int> vect;
	for (int i = 0; i < 3; ++i)
		vect.push_back(i);

	std::cout << "My vector = " << vect << std::endl;

	for (int i = 0; i <= vect.size(); ++i)
		std::cout << "Value of vector at " << i << " is " << vect[i] << std::endl;

	std::cout << "scpp_vector successful\n";
	return 0;
}