#ifndef ROLA_BASE_ASSIGNABLE_HPP
#define ROLA_BASE_ASSIGNABLE_HPP

#include <optional>

namespace rola
{
	namespace base
	{
		// wrap for lambda
		template <typename T>
		class Assignable
		{
		private:
			std::optional<T> value_;

		public:
			Assignable(T const& value)
				: value_(value)
			{}

			Assignable(T&& value)
				: value_(std::move(value))
			{}

			Assignable(Assignable const& other) = default;
			Assignable(Assignable&& other) = default;
			Assignable& operator=(Assignable const& other)
			{
				value_.emplace(*other.value_);
				return *this;
			}
			Assignable& operator=(Assignable&& other) = default;

		public:
			T const& get() const
			{
				return value_;
			}

			T& get()
			{
				return value_;
			}

			template <typename... Args>
			decltype(auto) operator()(Args&&... args)
			{
				return (*value_)(std::forward<Args>(args)...);
			}
		};

		// wrap for reference
		template <typename T>
		class Assignable<T&>
		{
		private:
			std::reference_wrapper<T> value_;

		public:
			explicit Assignable(T& value)
				: value_(value)
			{}

			T const& get() const
			{
				return value_;
			}
			
			T& get()
			{
				return value_;
			}

			template <typename... Args>
			decltype(auto) operator()(Args&&... args)
			{
				return value_(std::forward<Args>(args)...);
			}
		};
	} // namespace base
} // namespace rola

#endif // !ROLA_BASE_ASSIGNABLE_HPP
