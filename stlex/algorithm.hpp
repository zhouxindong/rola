#ifndef ROLA_STLEX_ALGORITHM_HPP
#define ROLA_STLEX_ALGORITHM_HPP

#include <vector>
#include <algorithm>
#include <utility>
#include <functional>
#include <map>
#include <queue>
#include <numeric>

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

	template <typename T>
	void insert_sorted(std::vector<T>& v, T const& val)
	{
		if (std::is_sorted(std::begin(v), std::end(v)))
		{
			const auto insert_pos = std::lower_bound(std::begin(v), std::end(v), val);
			v.insert(insert_pos, val);
		}
		else
		{
			v.insert(val);
		}		
	}

	template <typename InIt, typename OutIt, typename T, typename F>
	InIt split(InIt it, InIt end_it, OutIt out_it, T split_val, F bin_func)
	{
		while (it != end_it)
		{
			auto slice_end(std::find(it, end_it, split_val));
			*out_it++ = bin_func(it, slice_end);

			if (slice_end == end_it) return end_it;
			it = std::next(slice_end);
		}
		return it;
	}

	template <typename It, typename F>
	std::pair<It, It> gather(It first, It last, It gather_pos, F predicate)
	{
		return { std::stable_partition(first, gather_pos, std::not_fn(predicate)),
			std::stable_partition(gather_pos, last, predicate) };
	}

	template <typename It>
	It remove_multi_whitespace(It it, It end_it)
	{
		return std::unique(it, end_it, [](const auto& a, const auto& b) {
			return std::isspace(a) && std::isspace(b);
			});
	}

	// can assign and iterate
	template <typename F, typename R>
	R mapf(F&& f, R r)
	{
		std::transform(std::begin(r), std::end(r), std::begin(r),
			std::forward<F>(f));
		return r;
	}

	// can iterate but not assign
	template <typename F, typename T, typename U>
	std::map<T, U> mapf(F&& f, std::map<T, U> const& m)
	{
		std::map<T, U> r;
		for (auto const kvp : m)
			r.insert(f(kvp));
		return r;
	}

	// neither iterate nor assign
	template <typename F, typename T>
	std::queue<T> mapf(F&& f, std::queue<T> q)
	{
		std::queue<T> r;
		while (!q.empty())
		{
			r.push(f(q.front()));
			q.pop();
		}
		return r;
	}

	// can iterate
	template <typename F, typename R, typename T>
	constexpr T foldl(F&& f, R&& r, T i)
	{
		return std::accumulate(std::begin(r), std::end(r),
			std::move(i), std::forward<F>(f));
	}

	template <typename F, typename R, typename T>
	constexpr T foldr(F&& f, R&& r, T i)
	{
		return std::accumulate(
			std::rbegin(r), std::rend(r),
			std::move(i),
			std::forward<F>(f));
	}

	// can't iterate
	template <typename F, typename T>
	constexpr T foldl(F&& f, std::queue<T> q, T i)
	{
		while (!q.empty())
		{
			i = f(i, q.front());
			q.pop();
		}
		return i;
	}

	template <typename RandomAccessIterator>
	std::pair<RandomAccessIterator, RandomAccessIterator>
		slide(RandomAccessIterator first,
			RandomAccessIterator last,
			RandomAccessIterator pos)
	{
		if (pos < first)
			return { pos, std::rotate(pos, first, last) };
		if (last < pos)
			return { std::rotate(first, last, pos), pos };
		return { first, last };
	}



} // namespace rola

#endif // !ROLA_STLEX_ALGORITHM_HPP
