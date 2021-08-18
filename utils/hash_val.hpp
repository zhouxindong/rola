#ifndef ROLA_UTILS_HASH_VAL_HPP
#define ROLA_UTILS_HASH_VAL_HPP

#include <functional>

namespace rola
{
	template <typename T>
	inline void hash_combine(std::size_t& seed, T const& val)
	{
		seed ^= std::hash<T>()(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	template <typename T>
	inline void hash_val(std::size_t& seed, T const& val)
	{
		hash_combine(seed, val);
	}

	template <typename T, typename ...Types>
	inline void hash_val(std::size_t& seed, T const& val, Types const&... args)
	{
		hash_combine(seed, val);
		hash_val(seed, args...);
	}

	template <typename ...Types>
	inline std::size_t hash_val(Types const&... args)
	{
		std::size_t seed = 0;
		hash_val(seed, args...);
		return seed;
	}
} // namespace rola
#endif // !ROLA_UTILS_HASH_VAL_HPP