#include <stddef.h>
#include <utility>

// 1. assignable model
// 2. constructible model
template <class T, size_t N>
struct block
{
	T data[N];

	typedef T						value_type;
	typedef value_type*				pointer;
	typedef const value_type*		const_pointer;
	typedef value_type&				reference;
	typedef const value_type&		const_reference;

	typedef ptrdiff_t				difference_type;
	typedef size_t					size_type;

	typedef pointer					iterator;
	typedef const_pointer			const_iterator;

	iterator begin() { return data; }
	iterator end() { return data + N; }

	const_iterator begin() const { return data; }
	const_iterator end() const { return data + N; }

	reference operator[](size_type n) { return data[n]; }
	const_reference operator[](size_type n) const { return data[n]; }

	size_type size() const { return N; }
	size_type max_size() const { return N; }
	bool empty() const { return N == 0; }

	void swap(block& x)
	{
		for (size_t n = 0; n < N; ++n)
			std::swap(data[n], x.data[n]);
	}
};

// 3. equality comparable model
template <class T, size_t N>
bool operator==(const block<T, N>& x, const block<T, N>& y)
{
	for (size_t n = 0; n < N; ++n)
		if (x.data[n] != y.data[n])
			return false;
	return true;
}

// 4. lessthan comparable model
template <class T, size_t N>
bool operator<(const block<T, N>& x, const block<T, N>& y)
{
	for (size_t n = 0; n < N; ++n)
		if (x.data[n] < y.data[n])
			return true;
		else if (y.data[n] > x.data[n])
			return false;
	return false;
}