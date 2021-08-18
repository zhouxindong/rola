#include <iostream>
#include "scpp_matrix.hpp"

int main()
{
	rola::matrix<int>::size_type rows = 5;
	rola::matrix<int>::size_type cols = 10;

	rola::matrix<int> m{ rows, cols };

	for(int i=0; i<rows; ++i)
		for (int j = 0; j < cols; ++j)
		{
			m[i][j] = i * j;
		}

	std::cout << m << "\n";
	std::cout << "scpp_matrix successful\n";
	return 0;
}