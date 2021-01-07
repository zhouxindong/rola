#ifndef ROLA_LINGEX_FLUENTX_HPP
#define ROLA_LINGEX_FLUENTX_HPP

#include <memory>
#include "conditionable.hpp"
#include "loopable.hpp"

namespace rola
{
	namespace Flux
	{
		inline std::shared_ptr<Condition_builder> If(std::function<bool()> condition)
		{
			return std::make_shared<Condition_express>(condition);
		}

		inline std::shared_ptr<Condition_builder> If(bool condition)
		{
			return std::make_shared<Condition_express>(condition);
		}

		inline void While(std::function<bool()> cond, std::function<void()> action)
		{
			while (cond())
			{
				action();
			}
		}

		inline std::shared_ptr<Early_loop_builder> While(std::function<bool()> cond)
		{
			return std::make_shared<Loopable>(cond);
		}

		inline std::shared_ptr<Later_loop_builder> Do(std::function<void()> action)
		{
			return std::make_shared<Loopable>(action);
		}
	}
} // namespace rola
#endif // !ROLA_LINGEX_FLUENTX_HPP
