
#include <algorithm>

#pragma region fixed_vector

template <typename T, size_t Size>
class fixed_vector
{
	T* v_;

public:
	typedef T* iterator;
	typedef T const* const_iterator;

public:
	fixed_vector()
		: v_(new T[Size])
	{}

	~fixed_vector() noexcept
	{
		delete[] v_;
	}

	template <typename U, size_t USize>
	fixed_vector(fixed_vector<U, USize> const& other)
		: v_(new T[Size])
	{
		try
		{
			std::copy(other.begin(), other.begin() + std::min(Size, USize),
				begin());
		}
		catch (...)
		{
			delete[] v_;
			throw; // exception safe
		}
	}

	fixed_vector(fixed_vector const& other)
		: v_(new T[size])
	{
		try
		{
			std::copy(other.begin(), other.end(), begin());
		}
		catch (...)
		{
			delete[] v_;
			throw;
		}
	}

	fixed_vector(fixed_vector&& other)
		: v_(other.v_)
	{
		other.v_ = nullptr;
	}

	void swap(fixed_vector& other) noexcept
	{
		std::swap(v_, other.v_);
	}

	template <typename U, size_t USize>
	fixed_vector& operator=(fixed_vector<U, USize> const& other)
	{
		fixed_vector temp(other);
		swap(temp);
		return *this;
	}

	fixed_vector& operator=(fixed_vector const& other)
	{
		fixed_vector temp(other);
		swap(temp);
		return *this;
	}

	fixed_vector& operator=(fixed_vector&& other)
	{
		delete[] v_;
		v_ = other.v_;
		other.v_ = nullptr;
	}

	iterator begin()
	{
		return v_;
	}

	iterator end()
	{
		return v_ + Size;
	}

	const_iterator begin() const
	{
		return v_;
	}

	const_iterator end() const
	{
		return v_ + Size;
	}

	const_iterator cbegin() const
	{
		return v_;
	}

	const_iterator cend() const
	{
		return v_ + Size;
	}
};

#pragma endregion

#pragma region stack

#include "base/any_assert.hpp"
#include <utility>

template <typename T>
T* new_copy(T const* src, size_t src_size, size_t dst_size)
{
	ASSERT_ANY(dst_size >= src_size);
	T* dst = new T[dst_size];
	try
	{
		std::copy(src, src + src_size, dst);
	}
	catch (...)
	{
		delete[] dst;
		throw;
	}
	return dst;
}

template <typename T1, typename T2>
void construct(T1* p, T2&& value)
{
	new(p) T1(std::forward<T2>(value));
}

template <typename T>
void destroy(T* p)
{
	p->~T();
}

template <typename FwdIter>
void destroy(FwdIter first, FwdIter last)
{
	while (first != last)
	{
		destroy(&*first);
		++first;
	}
}

template <typename T>
class stack_impl
{
protected:
	T* v_;
	size_t size_;
	size_t used_;

protected:
	stack_impl(size_t size = 0)
		: v_(size == 0 ? nullptr : operator new(sizeof(T) * size))
		, size_(size)
		, used_(0)
	{}

	~stack_impl()
	{
		destroy(v_, v_ + used_);
		operator delete(v_);
	}

	void swap(stack_impl& other) noexcept
	{
		std::swap(v_, other.v_);
		std::swap(size_, other.size_);
		std::swap(used_, other.used_);
	}

	stack_impl(stack_impl const&) = delete;
	stack_impl& operator=(stack_impl const&) = delete;
};

template <typename T>
class stack : private stack_impl<T>
{
public:
	stack(size_t size = 0)
		: stack_impl<T>(size)
	{
	}

	~stack() = default;

	stack(stack const& other)
		: stack_impl<T>(other.used_)
	{
		while (used_ < other.used_)
		{
			construct(v_ + used_, other.v_[used_]);
			++used_;
		}
	}

	stack& operator=(stack temp)
	{
		swap(temp);
		return *this;
	}

	size_t count() const
	{
		return used_;
	}

	void push(T const& t)
	{
		if (used_ == size_)
		{
			stack temp(size_ * 2 + 1);
			while (temp.count() < used_)
			{
				temp.push(v[temp.count()]);
			}
			temp.push(t);
			swap(temp);
		}
		else
		{
			construct(v_ + used_, t);
			++used_;
		}
	}

	T& top()
	{
		if (used_ == 0)
			throw "empty stack";
		return v_[used_];
	}

	void pop()
	{
		if (used_ == 0)
			throw "pop from empty stack";
		else
		{
			--used_;
			destroy(v_ + used_);
		}
	}
};

#pragma endregion