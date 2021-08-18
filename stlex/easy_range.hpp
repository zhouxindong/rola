#ifndef ROLA_STLEX_EASY_RANGE_HPP
#define ROLA_STLEX_EASY_RANGE_HPP

#include <utility>

namespace rola
{
	template <typename Container>
	class Range
	{
		using Iterator = typename Container::iterator;

	public:
		Range(std::pair<Iterator, Iterator> range)
			: begin_(range.first)
			, end_(range.second)
		{}

		Iterator begin() { return begin_; }
		Iterator end() { return end_; }

	private:
		Iterator begin_;
		Iterator end_;
	};

} // namesapce rola

#endif // !ROLA_STLEX_EASY_RANGE_HPP
