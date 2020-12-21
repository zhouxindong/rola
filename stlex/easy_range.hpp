#ifndef ROLA_STLEX_EASY_RANGE_HPP
#define ROLA_STLEX_EASY_RANGE_HPP

#include <type_traits>
#include <iterator>

namespace rola
{
	// range type representing a pair of iterators
	template <typename Iter>
	class Easy_range
	{
		Iter begin_;
		Iter end_;

	public:
		Easy_range() = default;
		Easy_range(Iter begin, Iter end)
			: begin_(begin)
			, end_(end)
		{}

		Iter begin() const
		{
			return begin_;
		}

		Iter end() const
		{
			return end_;
		}
	};

	template <typename Iter>
	Easy_range<Iter> make_easy_range(Iter begin, Iter end)
	{
		return { begin, end };
	}

	template <typename T>
	class Easy_int_range
	{
		T begin_;
		T end_;
		static_assert(std::is_integral_v<T>, 
			"Easy_int_range can only be used with integral type");

	public:
		class Iterator
		{
			T current_;

			explicit Iterator(T a)
				: current_(a)
			{}

			friend class Easy_int_range<T>;

		public:
			typedef T value_type;
			typedef std::ptrdiff_t difference_type;
			typedef Iterator pointer;
			typedef T reference;
			typedef std::random_access_iterator_tag iterator_category;

			Iterator() = default;

			T operator*() const
			{
				return current_;
			}

			T operator[](difference_type offset) const
			{
				return current_ + offset;
			}

			Iterator& operator++()
			{
				++current_;
				return *this;
			}

			Iterator operator++(int)
			{
				return Iterator(current_++);
			}

			Iterator& operator--()
			{
				--current_;
				return *this;
			}

			Iterator operator--(int)
			{
				return Iterator(current_);
			}

			Iterator& operator+=(difference_type offset)
			{
				current_ += offset;
				return *this;
			}

			Iterator& operator-=(difference_type offset)
			{
				current_ -= offset;
				return *this;
			}

			Iterator operator+(difference_type offset) const
			{
				return Iterator(current_ + offset);
			}

			Iterator operator-(difference_type offset) const
			{
				return Iterator(current_ - offset);
			}

			friend Iterator operator+(difference_type offset, Iterator other)
			{
				return other + offset;
			}

			friend difference_type operator-(Iterator a, Iterator b)
			{
				return a.current_ - b.current_;
			}

			friend bool operator==(Iterator a, Iterator b)
			{
				return a.current_ == b.current_;
			}

			friend bool operator!=(Iterator a, Iterator b)
			{
				return a.current_ != b.current_;
			}

			friend bool operator>(Iterator a, Iterator b)
			{
				return a.current_ > b.current_;
			}

			friend bool operator<(Iterator a, Iterator b)
			{
				return a.current_ < b.current_;
			}

			friend bool operator>=(Iterator a, Iterator b)
			{
				return a.current_ >= b.current_;
			}

			friend bool operator<=(Iterator a, Iterator b)
			{
				return a.current_ <= b.current_;
			}
		};

		Easy_int_range(T begin, T end)
			: begin_(begin)
			, end_(end)
		{}

		Iterator begin() const
		{
			return Iterator(begin_);
		}

		Iterator end() const
		{
			return Iterator(end_);
		}
	};

	template <typename T, typename U>
	Easy_int_range<std::common_type_t<T, U>> irange(T begin, U end)
	{
		return { begin, end };
	}
} // namesapce rola

#endif // !ROLA_STLEX_EASY_RANGE_HPP
