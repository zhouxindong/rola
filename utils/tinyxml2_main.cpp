#include "tinyxml2.hpp"
#include <iostream>
#include <string.h>
#include "stlex/string.hpp"

int main_tiny()
{
	rola::Easy_tinyxml2 tiny("test_utf8.xml");
	assert(tiny.ready());
	assert(tiny.root());

	auto v = tiny.all("ID1");
	assert(v.size() == 1);

	v = tiny.all("ID");
	assert(v.size() == 13);
	assert(tiny.has_ancestry(v[12], "CORE_ID"));
	assert(!tiny.has_ancestry(v[10], "Hello"));

	auto p2 = tiny.string_attribute(v[0], "DESC");
	assert(p2.first);
	assert(p2.second == "VDR组播地址");
	
	v = tiny.all("ID2");
	assert(v.size() == 1);
	auto p = tiny.string_attribute(v[0], "DESC");
	assert(p.first);
	assert(p.second == "VDR ID");

	v = tiny.all("ID3");
	assert(v.size() == 1);
	p2 = tiny.string_attribute(v[0], "DESC");
	assert(p2.first);
	assert(p2.second == "运行控制指令");

	std::cout << "tinyxml2_main successful\n";

	return 0;
}