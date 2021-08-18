#ifndef ROLA_CSP_SENDER_HPP
#define ROLA_CSP_SENDER_HPP

#include "message.hpp"

namespace rola
{
	namespace csp2
	{
		// only reference to a queue and push a message into it
		class sender
		{
			queue* q;

		public:
			sender()
				: q(nullptr)
			{}

			explicit sender(queue* q_)
				: q(q_)
			{}

			template <typename Message>
			void send(Message const& msg)
			{
				if (q)
					q->push(msg);
			}
		};
	} // namespace csp2
} // namespace rola

#endif // !ROLA_CSP_SENDER_HPP
