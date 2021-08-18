#ifndef ROLA_STLEX_BIT_VECTOR_HPP
#define ROLA_STLEX_BIT_VECTOR_HPP

#include <vector>
#include <algorithm>
#include <stdexcept>

namespace rola
{
	class bit_vector
	{
		std::vector<bool> bv_;

	public:
		bit_vector() = default;

		bit_vector(std::vector<bool> const& bv)
			: bv_{ bv }
		{}

		bool operator[] (size_t const i)
		{
			return bv_[i];
		}

		inline bool any() const
		{
			for (auto b : bv_)
				if (b)
					return true;
			return false;
		}

		inline bool all() const
		{
			for (auto b : bv_)
				if (!b)
					return false;
			return true;
		}

		inline bool none() const
		{
			return !any();
		}

		inline size_t count() const
		{
			return std::count(bv_.cbegin(), bv_.cend(), true);
		}

		inline size_t size() const
		{
			return bv_.size();
		}

		inline bit_vector& add(bool const value)
		{
			bv_.push_back(value);
			return *this;
		}

		inline bit_vector& remove(size_t const index)
		{
			if (index >= bv_.size())
				throw std::out_of_range("index out of range");
			bv_.erase(bv_.begin() + index);
			return *this;
		}

		inline bit_vector& set(bool const value = true)
		{
			for (size_t i = 0; i < bv_.size(); ++i)
			{
				bv_[i] = value;
			}
			return *this;
		}

		inline bit_vector& set(size_t const index, bool const value = true)
		{
			if (index >= bv_.size())
				throw std::out_of_range("index out of range");
			bv_[index] = value;
			return *this;
		}

		inline bit_vector& reset()
		{
			for (size_t i = 0; i < bv_.size(); ++i)
				bv_[i] = false;
			return *this;
		}

		inline bit_vector& reset(size_t const index)
		{
			if (index >= bv_.size())
				throw std::out_of_range("index out of range");
			bv_[index] = false;
			return *this;
		}

		inline bit_vector& flip()
		{
			bv_.flip();
			return *this;
		}

		std::vector<bool>& date()
		{
			return bv_;
		}
	};
} // namespace rola

#endif // !ROLA_STLEX_BIT_VECTOR_HPP
