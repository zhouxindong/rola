#ifndef ROLA_MPL_ANY_VALUE_HPP
#define ROLA_MPL_ANY_VALUE_HPP

#include <typeinfo>
#include <utility>
#include <type_traits>

namespace rola
{

#pragma region Holder_base

	class Holder_base
	{
	public:
		virtual ~Holder_base() {}

	public:
		virtual std::type_info const& type() const = 0;
		virtual Holder_base* clone() const = 0;
	};

#pragma endregion

#pragma region Value_holder

	template <typename T>
	class Value_holder : public Holder_base
	{
	public:
		T held;

	public:
		template <typename U>
		Value_holder(U&& value)
			: held(std::forward<U>(value))
		{}

	public:
		std::type_info const& type() const override
		{
			return typeid(std::decay_t<T>);
		}

		Holder_base* clone() const override
		{
			return new Value_holder(held);
		}
	};

#pragma endregion

#pragma region Any_value

	class Any_value
	{
		Holder_base* holder_;

	public:
		Any_value() : holder_(nullptr) {}

		Any_value(Any_value const& rhs)
			: holder_(rhs.holder_ != nullptr ? rhs.holder_->clone() : nullptr)
		{}

		Any_value(Any_value&& rhs) noexcept
			: holder_(rhs.holder_)
		{
			rhs.holder_ = nullptr;
		}

		template <
			typename T,
			typename DT = std::decay_t<T>,
			typename = std::enable_if_t<!std::is_same_v<Any_value, DT>>>
			Any_value(T&& value)
			: holder_(new Value_holder<DT>(std::forward<T>(value)))
		{}

		~Any_value()
		{
			delete holder_;
		}

		void swap(Any_value& rhs)
		{
			std::swap(holder_, rhs.holder_);
		}

	public:
		Any_value& operator=(Any_value const& rhs) 
		{
			Any_value tmp(rhs);
			swap(tmp);
			return *this;
		}

		Any_value& operator=(Any_value&& rhs) noexcept
		{
			swap(rhs);
			return *this;
		}

		template <
			typename T,
			typename DT = std::decay_t<T>,
			typename = std::enable_if_t<!std::is_same_v<Any_value, DT>>>
			Any_value& operator=(T&& rhs)
		{
			delete holder_;
			holder_ = new Value_holder<DT>(std::forward<T>(rhs));
			return *this;
		}

		explicit operator bool() const
		{
			return holder_ != nullptr;
		}

	public:
		template <typename T>
		bool can_as() const
		{
			if (holder_ == nullptr)
				return false;

			return (typeid(std::decay_t<T>).hash_code() == type().hash_code());
		}

		template <typename T>
		bool can_as()
		{
			if (holder_ == nullptr)
				return false;

			return (typeid(std::decay_t<T>).hash_code() == type().hash_code());
		}

		template <typename T>
		T const& as() const
		{
			if (!can_as<T>())
				throw std::bad_cast();
			return static_cast<Value_holder<T>*>(holder_)->held;
		}

		template <typename T>
		T& as()
		{
			if (!can_as<T>())
				throw std::bad_cast();
			return static_cast<Value_holder<T>*>(holder_)->held;
		}

	public:
		std::type_info const& type() const
		{
			return holder_ ? holder_->type() : typeid(void);
		}
	};

	void swap(Any_value& a, Any_value& b) noexcept
	{
		a.swap(b);
	}

#pragma endregion

} // namespace rola

#endif // !ROLA_MPL_ANY_VALUE_HPP
