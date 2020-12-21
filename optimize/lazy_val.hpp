#ifndef ROLA_OPTIMIZE_LAZY_VAL_HPP
#define ROLA_OPTIMIZE_LAZY_VAL_HPP

#include <mutex>

namespace rola
{
	template <typename F>
	class lazy_val
	{
	private:
		F computation_; 
		mutable decltype(computation_()) cache_;
		mutable std::once_flag value_flag_;

	public:
		lazy_val(F computation) noexcept
			: computation_(computation)
		{}

		lazy_val(lazy_val&& other) noexcept
			: computation_(std::move(other.computation_))
		{}

		operator decltype(computation_()) const& () const
		{
			std::call_once(value_flag_, [this] {
				cache_ = computation_();
				});
			return cache_;
		}
	};

	template <typename F>
	inline lazy_val<F> make_lazy_val(F&& computation)
	{
		return lazy_val<F>(std::forward<F>(computation));
	}

	struct _make_lazy_val_helper {
		template <typename F>
		auto operator - (F&& function) const
		{
			return lazy_val<F>(function);
		}
	} make_lazy_val_helper;

#define lazy rola::make_lazy_val_helper - [=]
} // namespace rola

#endif // !ROLA_OPTIMIZE_LAZY_VAL_HPP
