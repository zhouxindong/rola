#include <iostream>

void show_compiler()
{
#if defined _MSC_VER
	std::cout << "Visual C++" << std::endl;
#elif defined __clang__
	std::cout << "Clang" << std::endl;
#elif defined __GNUG__
	std::cout << "GCC" << std::endl;
#else
	std::cout << "Unknown compiler" << std::endl;
#endif
}

void show_architecture()
{
#if defined _MSC_VER
	std::cout <<
#if defined _M_X64
		"AMD64"
#elif defined _M_IX86
		"INTEL x86"
#elif defined _M_ARM
		"ARM"
#else
		"unknown"
#endif
		<< std::endl;

#elif defined __clang__ || __GNUG__
	std::cout <<
#if defined __amd64__
		"AMD64"
#elif defined __i386__
		"INTEL x86"
#elif defined __arm__
		"ARM"
#else
		"unknown"
#endif
		<< std::endl;
#else
#error Unknown compiler
#endif
}

void show_configuration()
{
	std::cout <<
#ifdef _DEBUG
		"debug"
#else
		"release"
#endif // _DEBUG
		<< std::endl;
}

int main_preprocessor()
{
	show_compiler();
	show_architecture();
	show_configuration();

	std::cout << "preprocessor main successful\n";
	return 0;
}