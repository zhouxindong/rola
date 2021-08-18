#ifndef ROLA_STLEX_PARALLEL_HPP
#define ROLA_STLEX_PARALLEL_HPP

#include <thread>
#include <algorithm>
#include <vector>
#include <numeric>
#include <future>

namespace rola
{
	inline unsigned get_no_of_threads()
	{
		return std::thread::hardware_concurrency();
	}

	template <typename Iter, typename F, size_t SequalSize = 10000>
	inline void parallel_map(Iter begin, Iter end, F f)
	{
		auto size = std::distance(begin, end);
		if (size <= SequalSize)
			std::transform(begin, end, begin, std::forward<F>(f));
		else
		{
			auto no_of_threads = get_no_of_threads();
			auto part = size / no_of_threads;
			auto last = begin;

			std::vector<std::thread> threads;
			for (unsigned i = 0; i < no_of_threads; ++i)
			{
				if (i == no_of_threads - 1)
					last = end;
				else
					std::advance(last, part);

				threads.emplace_back([=, &f] {std::transform(begin, last, begin, std::forward<F>(f)); });
				begin = last;
			}

			for (auto& t : threads)
				t.join();
		}
	}

	template <typename Iter, typename F, size_t SequalSize = 10000>
	inline void parallel_map2(Iter begin, Iter end, F f)
	{
		auto size = std::distance(begin, end);
		if (size <= SequalSize)
			std::transform(begin, end, begin, std::forward<F>(f));
		else
		{
			auto no_of_threads = get_no_of_threads();
			auto part = size / no_of_threads;
			auto last = begin;

			std::vector<std::future<void>> tasks;
			for (unsigned i = 0; i < no_of_threads; ++i)
			{
				if (i == no_of_threads - 1)
					last = end;
				else
					std::advance(last, part);

				tasks.emplace_back(std::async(
					std::launch::async,
					[=, &f] {std::transform(begin, last, begin, std::forward<F>(f)); }));
				begin = last;
			}

			for (auto& t : tasks)
				t.wait();
		}
	}

	template <typename Iter, typename R, typename F, size_t SequalSize = 10000>
	inline auto parallel_reduce(Iter begin, Iter end, R init, F op)
	{
		auto size = std::distance(begin, end);

		if (size <= 10000)
		{
			return std::accumulate(begin, end, init, std::forward<F>(op));
		}
		else
		{
			auto no_of_threads = get_no_of_threads();
			auto part = size / no_of_threads;
			auto last = begin;

			std::vector<std::thread> threads;
			std::vector<R> values(no_of_threads);
			for (unsigned i = 0; i < no_of_threads; ++i)
			{
				if (i == no_of_threads - 1)
					last = end;
				else
					std::advance(last, part);

				threads.emplace_back([=, &op](R& result) {
					result = std::accumulate(begin, last, R{}, std::forward<F>(op));
					}, std::ref(values[i]));
				begin = last;
			}

			for (auto& t : threads)
				t.join();

			return std::accumulate(std::begin(values), std::end(values), init, std::forward<F>(op));
		}
	}

	template <typename Iter, typename R, typename F, size_t SequalSize = 10000>
	inline auto parallel_reduce2(Iter begin, Iter end, R init, F op)
	{
		auto size = std::distance(begin, end);

		if (size <= 10000)
		{
			return std::accumulate(begin, end, init, std::forward<F>(op));
		}
		else
		{
			auto no_of_threads = get_no_of_threads();
			auto part = size / no_of_threads;
			auto last = begin;

			std::vector<std::future<R>> tasks;
			for (unsigned i = 0; i < no_of_threads; ++i)
			{
				if (i == no_of_threads - 1)
					last = end;
				else
					std::advance(last, part);

				tasks.emplace_back(std::async(
					std::launch::async, 
					[=, &op]() {
						return std::accumulate(begin, last, R{}, std::forward<F>(op)); }));
				begin = last;
			}

			std::vector<R> values;
			for (auto& t : tasks)
				values.push_back(t.get());

			return std::accumulate(std::begin(values), std::end(values), init, std::forward<F>(op));
		}
	}
} // namespace rola

#endif // !ROLA_STLEX_PARALLEL_HPP
