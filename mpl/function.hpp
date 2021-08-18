#ifndef ROLA_MPL_FUNCTION_HPP
#define ROLA_MPL_FUNCTION_HPP

#include <type_traits>

namespace rola
{
#pragma region Function_base

	template <typename R, typename... Args>
	class Function_base
	{
	public:
		virtual ~Function_base() {}

	public:
		virtual Function_base* clone() const = 0;
		virtual R call(Args... args) const = 0;
	};

#pragma endregion

#pragma region Function_wrap

	template <typename C, typename R, typename... Args>
	class Function_wrap : public Function_base<R, Args...>
	{
		C callable_;

	public:
		template <typename U>
		Function_wrap(U&& f)
			: callable_(std::forward<U>(f))
		{}

		Function_base* clone() const override
		{
			return new Function_wrap(callable_);
		}

		R call(Args... args) const override
		{
			return callable_(args...);
		}
	};

#pragma endregion

#pragma region Function

	template <typename Signature>
	class Function;

	// can not support class member function
	template <typename R, typename... Args>
	class Function<R(Args...)>
	{
		Function_base<R, Args...>* holder_;

		friend void swap(Function& a, Function& b)
		{
			std::swap(a.holder_, b.holder_);
		}

	public:
		Function() noexcept
			: holder_(nullptr)
		{}

		Function(Function const& rhs)
			: holder_(rhs.holder_ == nullptr ? nullptr : rhs.holder_->clone())
		{}

		Function(Function& rhs) noexcept
			: Function(static_cast<Function const&>(rhs))
		{}

		Function(Function&& rhs) noexcept
			: holder_(rhs.holder_)
		{
			rhs.holder_ = nullptr;
		}

		template <typename C>
		Function(C&& c)
			: holder_(nullptr)
		{
			using Functor = std::decay_t<C>; // handle deduce to a reference type
			using Holder = Function_wrap<Functor, R, Args...>;
			holder_ = new Holder(std::forward<C>(c));
		}

		~Function()
		{
			delete holder_;
		}

	public:
		Function& operator=(Function const& rhs)
		{
			Function tmp(rhs);
			swap(*this, tmp);

			return *this;
		}

		Function& operator=(Function& rhs)
		{
			this->operator=(static_cast<Function const&>(rhs));
			return *this;
		}

		Function& operator=(Function&& rhs) noexcept
		{
			delete holder_;
			holder_ = rhs.holder_;
			rhs.holder_ = nullptr;
			return *this;
		}

		template <typename C>
		Function& operator=(C&& c)
		{
			Function tmp(std::forward<C>(c));
			swap(*this, tmp);
			return *this;
		}

	public:
		explicit operator bool() const
		{
			return holder_ != nullptr;
		}

		R operator()(Args... args) const
		{
			return holder_->call(args...);
		}
	};

#pragma endregion
} // namespace rola

#endif // !ROLA_MPL_FUNCTION_HPP
