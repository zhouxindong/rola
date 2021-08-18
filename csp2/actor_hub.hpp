#ifndef ROLA_CSP2_ACTOR_HUB_HPP
#define ROLA_CSP2_ACTOR_HUB_HPP

#include "receiver.hpp"

namespace rola
{
	class Actor_core
	{
	protected:
		Actor_core() = default;
		~Actor_core() = default;

		Actor_core(Actor_core const&) = delete;
		Actor_core& operator=(Actor_core const&) = delete;

		Actor_core(Actor_core&&) = default;
		Actor_core& operator=(Actor_core&&) = default;

	public:
		csp2::sender get_sender()
		{
			return incoming_;
		}

		void done()
		{
			get_sender().send(csp2::close_queue());
		}

	private:
		csp2::receiver incoming_;
	};
} // namespace rola

#endif // ROLA_CSP2_ACTOR_HUB_HPP
