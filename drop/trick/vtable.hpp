#include <iostream>

struct vtable
{
	void (*speak)(void* ptr);
	void (*destroy_)(void* ptr);

	void* (*clone_)(void* ptr);
	void* (*move_clone_)(void* ptr);
};

template <typename Concrete>
constexpr vtable vtable_for
{
	[](void* ptr) {static_cast<Concrete*>(ptr)->speak(); },
	[](void* ptr) {delete static_cast<Concrete*>(ptr); },
	[](void* ptr) -> void*
		{ return new Concrete(*static_cast<Concrete*>(ptr)); },
	[](void* ptr) -> void*
		{ return new Concrete(std::move(*static_cast<Concrete*>(ptr))); }
};

struct animal
{
	void* concrete_;
	vtable const* vtable_;

	template <typename T>
	animal(T const& t)
		: concrete_(new T(t))
		, vtable_(&vtable_for<T>)
	{}

	animal(animal const& rhs)
		: concrete_(rhs.vtable_->clone_(rhs.concrete_))
		, vtable_(rhs.vtable_)
	{}

	animal(animal&& rhs) noexcept
		: concrete_(rhs.vtable_->move_clone_(rhs.concrete_))
		, vtable_(rhs.vtable_)
	{}

	animal& operator=(animal const& rhs)
	{
		concrete_ = rhs.vtable_->clone_(rhs.concrete_);
		vtable_ = rhs.vtable_;
		return *this;
	}

	animal& operator=(animal&& rhs) noexcept
	{
		concrete_ = rhs.vtable_->move_clone_(rhs.concrete_);
		vtable_ = rhs.vtable_;
		return *this;
	}

	void speak()
	{
		vtable_->speak(concrete_);
	}

	~animal()
	{
		vtable_->destroy_(concrete_);
	}
};

struct cat
{
	void speak()
	{
		std::cout << "cat::speak()\n";
	}
};

struct dog
{
	void speak()
	{
		std::cout << "dog::speak()\n";
	}
};

