#ifndef ROLA_SAFECPP_SCPP_ARRAY_HPP
#define ROLA_SAFECPP_SCPP_ARRAY_HPP

#include <initializer_list>

#include "scpp_assert.hpp"

namespace rola
{
	template <typename T, unsigned N>
	class array
	{
	public:
		typedef unsigned size_type;

		array() {}
		explicit array(const T& initial_value)
		{
			for (size_type i = 0; i < N; ++i)
				data_[i] = initial_value;
		}

		array(const std::initializer_list<T>& l)
		{
			typename std::initializer_list<T>::iterator it = l.begin();

			for (size_type i = 0; i < N; ++i)
			{
				if (it != l.end())
				{
					data_[i] = *it;
					++it;
				}
				else
					break;
			}				
		}

		size_type size() const
		{
			return N;
		}

		T& operator[](size_type index)
		{
			SCPP_ASSERT(index < N,
				"Index " << index << " must be less than " << N);
			return data_[index];
		}

		const T& operator[](size_type index) const
		{
			SCPP_ASSERT(index < N,
				"Index " << index << " must be less than " << N);
			return data_[index];
		}

		T* begin() { return &data_[0]; }
		const T* begin() const { return &data_[0]; }

		T* end() { return &data_[N]; }
		const T* end() const { return &data_[N]; }

	private:
		T data_[N];
	};

	template <typename T, unsigned N>
	inline std::ostream& operator<<(std::ostream& os, const rola::array<T, N>& a)
	{
		for (unsigned i = 0; i < N; ++i)
		{
			os << a[i];
			if (i + 1 < N)
				os << " ";
		}
		return os;
	}
} // namespace rola

#endif // ROLA_SAFECPP_SCPP_ARRAY_HPP
