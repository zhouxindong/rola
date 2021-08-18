#ifndef ROLA_SAFECPP_SCPP_PTR_HPP
#define ROLA_SAFECPP_SCPP_PTR_HPP

#include "scpp_assert.hpp"

namespace rola
{
	template <typename T>
	class not_null_ptr
	{
	public:
		explicit not_null_ptr(T* p)
			: ptr_(p)
		{
			SCPP_ASSERT(ptr_ != nullptr, "construct a not_null_ptr object with null ptr");
		}

		T* get() const
		{
			return ptr_;
		}

		not_null_ptr& operator=(T* p)
		{
			SCPP_ASSERT(p != nullptr, "can't assign null to a not_null_ptr object");
			ptr_ = p;
			return *this;
		}

		T* operator->() const
		{
			return ptr_;
		}

		T& operator*() const
		{
			return *ptr_;
		}

	private:
		T* ptr_;
	};

	/// <summary>
	/// scoped pointer, takes ownership of an object, could not be copied
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	class scoped_ptr
	{
	public:
		explicit scoped_ptr(T* p = nullptr)
			: ptr_(p)
		{}

		scoped_ptr(const scoped_ptr&)				= delete;
		scoped_ptr& operator=(const scoped_ptr&)	= delete;

		scoped_ptr& operator=(T* p)
		{
			if (ptr_ != p)
			{
				delete ptr_;
				ptr_ = p;
			}
			return *this;
		}

		~scoped_ptr()
		{
			if (ptr_ != nullptr)
				delete ptr_;
		}

		T* get() const
		{
			return ptr_;
		}

		T* operator->() const
		{
			SCPP_ASSERT(ptr_ != nullptr, "attempt to use operator-> on null pointer.");
			return ptr_;
		}

		T& operator*() const
		{
			SCPP_ASSERT(ptr_ != nullptr, "attempt to use operator* on null pointer.");
			return *ptr_;
		}

		// release ownership of the object to the caller
		T* release()
		{
			T* p = ptr_;
			ptr_ = nullptr;
			return p;
		}

	private:
		T* ptr_;
	};
} // namespace rola

#endif // ROLA_SAFECPP_SCPP_PTR_HPP
