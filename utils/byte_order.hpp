#ifndef ROLA_UTILS_BYTE_ORDER_HPP
#define ROLA_UTILS_BYTE_ORDER_HPP

#include <stdint.h>
#include <iostream>
#include <string.h>

namespace rola
{
#pragma region byte order defines

	namespace bo_detail
	{
		enum class ByteOrder
		{
			BO_UNKNOWN,
			BO_BIG_ENDIAN,
			BO_LITTLE_ENDIAN
		};

		union Order_detector
		{
		private:
			int32_t int_data;
			int8_t array_data[sizeof(int32_t)];

			Order_detector()
				: int_data(1)
			{
			}

		public:
			static ByteOrder get_order()
			{
				Order_detector od;
				if (od.array_data[3] == 1 && od.array_data[0] == 0)
					return ByteOrder::BO_BIG_ENDIAN;
				else if (od.array_data[0] == 1 && od.array_data[3] == 0)
					return ByteOrder::BO_LITTLE_ENDIAN;
				else
					return ByteOrder::BO_UNKNOWN;
			}
		};
	} // namespace bo_detail

	inline bool host_is_bigendian()
	{
		return bo_detail::Order_detector::get_order() == bo_detail::ByteOrder::BO_BIG_ENDIAN;
	}

#pragma endregion

#pragma region reverse byte order

	namespace bo_detail
	{
		template <int>
		struct Integer_for_size;

		template <>
		struct Integer_for_size<1>
		{
			using Unsigned = uint8_t;
			using Signed = int8_t;
		};

		template <>
		struct Integer_for_size<2>
		{
			using Unsigned = uint16_t;
			using Signed = int16_t;
		};

		template <>
		struct Integer_for_size<4>
		{
			using Unsigned = uint32_t;
			using Signed = int32_t;
		};

		template <>
		struct Integer_for_size<8>
		{
			using Unsigned = uint64_t;
			using Signed = int64_t;
		};

		template <typename T>
		struct Integer_for_sizeof : Integer_for_size<sizeof(T)> {};
	} // namespace bo_detail

	template <typename T>
	constexpr T reverse_byteorder(T source)
	{
		return source;
	}

	template <>
	inline constexpr uint64_t reverse_byteorder<uint64_t>(uint64_t source)
	{
		return 0
			| ((source & 0x00000000000000ffull) << 56)
			| ((source & 0x000000000000ff00ull) << 40)
			| ((source & 0x0000000000ff0000ull) << 24)
			| ((source & 0x00000000ff000000ull) << 8)
			| ((source & 0x000000ff00000000ull) >> 8)
			| ((source & 0x0000ff0000000000ull) >> 24)
			| ((source & 0x00ff000000000000ull) >> 40)
			| ((source & 0xff00000000000000ull) >> 56);
	}

	template <>
	inline constexpr uint32_t reverse_byteorder<uint32_t>(uint32_t source)
	{
		return 0
			| ((source & 0x000000ff) << 24)
			| ((source & 0x0000ff00) << 8)
			| ((source & 0x00ff0000) >> 8)
			| ((source & 0xff000000) >> 24);
	}

	template <>
	inline constexpr uint16_t reverse_byteorder<uint16_t>(uint16_t source)
	{
		return uint16_t(
			0
			| ((source & 0x00ff) << 8)
			| ((source & 0xff00) >> 8)
		);
	}

	template <>
	inline constexpr int64_t reverse_byteorder<int64_t>(int64_t source)
	{
		return reverse_byteorder<uint64_t>(uint64_t(source));
	}

	template <>
	inline constexpr int32_t reverse_byteorder<int32_t>(int32_t source)
	{
		return reverse_byteorder<uint32_t>(uint32_t(source));
	}

	template <>
	inline constexpr int16_t reverse_byteorder<int16_t>(int16_t source)
	{
		return reverse_byteorder<uint16_t>(uint16_t(source));
	}

	template <typename T>
	inline void to_unaligned(const T src, void* dest)
	{
		::memcpy(dest, &src, sizeof(T));
	}

	template <typename T>
	inline T from_unaligned(const void* src)
	{
		T dest;
		::memcpy(&dest, src, sizeof(T));
		return dest;
	}

	namespace bo_detail
	{
		template <typename Float>
		Float reverse_float_helper(Float source)
		{
			auto temp = from_unaligned<typename Integer_for_sizeof<Float>::Unsigned>(&source);
			temp = reverse_byteorder(temp);
			return from_unaligned<Float>(&temp);
		}
	} // namespace bo_detail

	inline float reverse_byteorder(float source)
	{
		return bo_detail::reverse_float_helper(source);
	}

	inline double reverse_byteorder(double source)
	{
		return bo_detail::reverse_float_helper(source);
	}

#pragma endregion

#pragma region big/little endian

	template <typename T>
	inline constexpr T host_to_network(T source)
	{
		if (host_is_bigendian())
			return source;
		return reverse_byteorder(source);
	}

	template <typename T>
	inline void host_to_network(const T source, void* dest)
	{
		T src_n = host_to_network(source);
		to_unaligned(src_n, dest);
	}

	template <typename T>
	inline constexpr T network_to_host(T source)
	{
		return host_to_network(source);
	}

	template <typename T>
	inline constexpr T network_to_host(const void* src)
	{
		T data = from_unaligned<T>(src);
		return network_to_host(data);
	}

	template <typename T>
	inline constexpr T to_big_endian(T source)
	{
		return host_to_network(source);
	}

	template <typename T>
	inline constexpr T from_big_endian(T source)
	{
		return to_big_endian(source);
	}

	template <typename T>
	inline constexpr T to_little_endian(T source)
	{
		if (host_is_bigendian())
			return reverse_byteorder(source);

		return source;
	}

	template <typename T>
	inline constexpr T from_little_endian(T source)
	{
		return to_little_endian(source);
	}
#pragma endregion
} // namespace rola

#endif // !ROLA_UTILS_BYTE_ORDER_HPP
