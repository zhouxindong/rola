#include "singlable.hpp"
using namespace rola::base;
class Foo
{
public:
	Foo(int ii, double dd) : i(ii), d(dd) {
		//pi = &i;
	}
	int i;
	double d;
	//int* pi;

	//bool operator==(Foo const& rhs)
	//{
	//	return i == rhs.i && d == rhs.d && pi == rhs.pi;
	//}
};

void bar()
{
	Foo* f = Singlable<Foo>::instance_p(3, 2.33);
}