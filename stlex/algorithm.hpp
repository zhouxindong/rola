#ifndef ROLA_STLEX_ALGORITHM_HPP
#define ROLA_STLEX_ALGORITHM_HPP

#include <vector>

namespace rola
{
	template <typename T>
	void quick_remove_at(std::vector<T>& v, std::size_t idx)
	{
		if (idx < v.size())
		{
			v[idx] = std::move(v.back());
			v.pop_back();
		}
	}

	template <typename T>
	void quick_remove_at(std::vector<T>& v,
		typename std::vector<T>::iterator it)
	{
		if (it != std::end(v))
		{
			*it = std::move(v.back());
			v.pop_back();
		}
	}
} // namespace rola

#endif // !ROLA_STLEX_ALGORITHM_HPP
