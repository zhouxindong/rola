#include <exception>

/// <summary>
/// demo for how to let costum class can be use for_range
/// </summary>
/// <typeparam name="T"></typeparam>

template <typename T, size_t const Size>
class dummy_array
{
	T data[Size] = {};

public:
	T const& GetAt(size_t const index) const
	{
		if (index < Size) return data[index];
		throw std::out_of_range("index out of range");
	}

	void SetAt(size_t const index, T const& value)
	{
		if (index < Size) data[index] = value;
		else throw std::out_of_range("index out of range");
	}

	size_t GetSize() const { return Size; }
};

/// <summary>
/// create mutable and constant itreators for dummy_array
/// 1. operator++
/// 2. operator*
/// 3. operator!=
// begin, end
/// </summary>
/// <typeparam name="T"></typeparam>
/// <typeparam name="C"></typeparam>
template <typename T, typename C, size_t const Size>
class dummy_array_iterator_type
{
public:
	dummy_array_iterator_type(C& collection, size_t const index)
		: index(index)
		, collection(collection)
	{}

	bool operator!=(dummy_array_iterator_type const& other) const
	{
		return index != other.index;
	}

	T const& operator*() const
	{
		return collection.GetAt(index);
	}

	dummy_array_iterator_type const& operator++()
	{
		++index;
		return *this;
	}

private:
	size_t index;
	C& collection;
};

template <typename T, size_t const Size>
using dummy_array_iterator = dummy_array_iterator_type<T, dummy_array<T, Size>, Size>;

template <typename T, size_t const Size>
using dummy_array_const_iterator = dummy_array_iterator_type<T, dummy_array<T, Size> const, Size>;

// begin(), end()

template <typename T, size_t const Size>
inline dummy_array_iterator<T, Size> begin(dummy_array<T, Size>& collection)
{
	return dummy_array_iterator<T, Size>(collection, 0);
}

template <typename T, size_t const Size>
inline dummy_array_iterator<T, Size> end(dummy_array<T, Size>& collection)
{
	return dummy_array_iterator<T, Size>(collection, collection.GetSize());
}

template <typename T, size_t const Size>
inline dummy_array_const_iterator<T, Size> begin(dummy_array<T, Size> const& collection)
{
	return dummy_array_const_iterator<T, Size>(collection, 0);
}

template <typename T, size_t const Size>
inline dummy_array_const_iterator<T, Size> end(dummy_array<T, Size> const& collection)
{
	return dummy_array_const_iterator<T, Size>(collection, collection.GetSize());
}