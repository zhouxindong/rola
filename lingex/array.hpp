#ifndef ROLA_LINGEX_ARRAY_HPP
#define ROLA_LINGEX_ARRAY_HPP

namespace rola
{
	namespace lingex
	{
		template <typename T, size_t N, typename U, size_t M>
		inline bool value_equals(T(&a)[N], U(&b)[M])
		{
			if (N != M)
				return false;

			for (size_t i = 0; i < N; ++i)
				if (a[i] != b[i])
					return false;
			return true;
		}
	} // namespace lingex
} // namespace rola

#endif // !ROLA_LINGEX_ARRAY_HPP
