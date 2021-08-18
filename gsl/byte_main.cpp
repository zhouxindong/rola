#include <iostream>
#include "byte.hpp"
#include <cassert>

int main_byte()
{
	// construction
	{
		const gsl::byte b = static_cast<gsl::byte>(4);
		assert(4 == static_cast<unsigned char>(b));
	}

	{
		const gsl::byte b = gsl::byte(12);
		assert(12 == static_cast<unsigned char>(b));
	}

	{
		const gsl::byte b = gsl::to_byte<12>();
		assert(12 == static_cast<unsigned char>(b));
	}

#if defined(__cplusplus) && (__cplusplus >= 201703L)
	{
		const byte b{ 14 };
		EXPECT_TRUE(static_cast<unsigned char>(b) == 14);
	}
#endif

	// bitwise_operations
	{
		const gsl::byte b = gsl::to_byte<0xFF>();
		gsl::byte a = gsl::to_byte<0x00>();

		assert((b | a) == b);
		assert(a == gsl::to_byte<0x00>());

		a |= b;
		assert(a == gsl::to_byte<0xFF>());

		using namespace gsl;

		a = to_byte<0x01>();
		assert((b & a) == to_byte<0x01>());

		a &= b;
		assert(a == to_byte<0x01>());

		assert((b ^ a) == to_byte<0xFE>());

		assert(a == to_byte<0x01>());
		a ^= b;
		assert(a == to_byte<0xFE>());

		a = to_byte<0x01>();
		assert(~a == to_byte<0xFE>());

		a = to_byte<0xFF>();
		assert((a << 4) == to_byte<0xF0>());
		assert((a >> 4) == to_byte<0x0F>());

		a <<= 4;
		assert(a == to_byte<0xF0>());
		a >>= 4;
		assert(a == to_byte<0x0F>());
	}

	{
		using namespace gsl;

		const byte b = to_byte<0x12>();

		assert(0x12 == gsl::to_integer<char>(b));
		assert(0x12 == gsl::to_integer<short>(b));
		assert(0x12 == gsl::to_integer<long>(b));
		assert(0x12 == gsl::to_integer<long long>(b));

		assert(0x12 == gsl::to_integer<unsigned char>(b));
		assert(0x12 == gsl::to_integer<unsigned short>(b));
		assert(0x12 == gsl::to_integer<unsigned long>(b));
		assert(0x12 == gsl::to_integer<unsigned long long>(b));

		//      assert(0x12 == gsl::to_integer<float>(b));   // expect compile-time error
		//      assert(0x12 == gsl::to_integer<double>(b));  // expect compile-time error
	}

	std::cout << "byte_main successful\n";
	return 0;
}