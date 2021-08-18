#ifndef ROLA_SAFECPP_SCPP_VECTOR_HPP
#define ROLA_SAFECPP_SCPP_VECTOR_HPP

#include <vector>
#include "scpp_assert.hpp"

namespace rola
{
	template <typename T>
	class vector : public std::vector<T>
	{
	public:
		typedef unsigned size_type;

		explicit vector(size_type n = 0)
			: std::vector<T>(n)
		{}

		vector(size_type n, const T& value)
			: std::vector<T>(n, value)
		{}

		template <class InputIterator>
		vector(InputIterator first, InputIterator last)
			: std::vector<T>(first, last)
		{}

		T& operator[](size_type index)
		{
			SCPP_ASSERT(index < std::vector<T>::size(),
				"Index " << index << " must be less than "
				<< std::vector<T>::size());
			return std::vector<T>::operator[](index);
		}

		const T& operator[](size_type index) const
		{
			SCPP_ASSERT(index < std::vector<T>::size(),
				"Index " << index << " must be less than "
				<< std::vector<T>::size());
			return std::vector<T>::operator[](index);
		}
	};

	template <typename T>
	inline std::ostream& operator<<(std::ostream& os, const rola::vector<T>& v)
	{
		for (unsigned i = 0; i < v.size(); ++i)
		{
			os << v[i];
			if (i + 1 < v.size())
				os << " ";
		}
		return os;
	}
} // namespace rola

#endif // ROLA_SAFECPP_SCPP_VECTOR_HPP
