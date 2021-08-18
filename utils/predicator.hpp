#ifndef ROLA_UTILS_PREDICATOR_HPP
#define ROLA_UTILS_PREDICATOR_HPP

#include <vector>
#include <functional>

namespace rola
{
	template <typename T>
	class Predicator
	{
	public:
		template <typename F>
		Predicator& add(F&& f)
		{
			ps_.push_back(std::forward<F>(f));
			return *this;
		}

		bool operator()(T const& d) const
		{
			for (auto& p : ps_)
			{
				if (!p(d))
					return false;
			}
			return true;
		}
	private:
		std::vector<std::function<bool(T const&)>> ps_;
	};
} // namespace rola

#endif // !ROLA_UTILS_PREDICATOR_HPP