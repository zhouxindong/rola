#include <iostream>
#include "stack_tracer.hpp"

void foo()
{
	throw std::logic_error("logic error");
	//std::cout << rola::print_stacktrace("prefix", "suffix");
}

void bar()
{
	foo();
}

void f()
{
	bar();
}

void g()
{
	f();
}

void h()
{
	g();
}

int main_stacktracer()
{
	using namespace rola;
	h();



	std::cout << "stack tracer successful\n";
	return 0;
}