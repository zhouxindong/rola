#ifndef ROLA_LINGEX_NAMED_TYPE_HPP
#define ROLA_LINGEX_NAMED_TYPE_HPP

#include <utility>
#include <string>

namespace rola
{
	template <typename T, typename Parameter>
	class Named_type
	{
	public:
		explicit Named_type(T const& value)
			: value_(value)
		{}

		explicit Named_type(T&& value)
			: value_(std::move(value))
		{}

	public:
		T& get()
		{
			return value_;
		}

		T const& get() const
		{
			return value_;
		}

	private:
		T value_;
	};

	//using Width = Named_type<double, struct Width_parameter>;

	template <typename T, typename Parameter>
	class Named_type_ref
	{
	public:
		explicit Named_type_ref(T& t)
			: t_(std::ref(t))
		{}

		T& get() { return t_.get(); }
		T const& get() const { return t_.get(); }
	private:
		std::reference_wrapper<T> t_;
	};

	//using NameRef = Named_type_ref<std::string, struct Name_ref_parameter>;
} // namespace rola
#endif // !ROLA_LINGEX_NAMED_TYPE_HPP
