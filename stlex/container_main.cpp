#include "container.hpp"

#include <vector>
#include <iostream>
#include <cassert>
#include <map>
#include <string>

int main_cm()
{
	std::vector<int> v{ 1,2,3,4,5,6 };
	rola::stlex::erase_remove(v, [](int const& i) {
		return i % 2 == 0;
		});
	assert(v.size() == 3);
	assert(v[0] == 1);
	assert(v[1] == 3);
	assert(v[2] == 5);

	std::map<std::string, int> m{ {"1", 1}, {"2", 2}, {"3", 3} };
	rola::stlex::replace_key(m, "2", "two");
	assert(m["1"] == 1);
	assert(m["two"] == 2);
	assert(m["3"] == 3);
	assert(m.size() == 3);

	std::cout << "container test successful\n";
	return 0;
}