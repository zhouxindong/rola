#ifndef ROLA_CSP_RECEIVER_HPP
#define ROLA_CSP_RECEIVER_HPP

#include "dispatcher.hpp"
#include "sender.hpp"

namespace rola
{
	namespace csp
	{
		// own the message queue
		// 1. wait for queue message
		// 2. check message type if type matched, then dispatch(schdule the message)
		class receiver
		{
			queue q; // receiver has own the queue

		public:
			operator sender() // can be implicit convert to the sender of queue
			{
				return sender(&q);
			}

			dispatcher wait() // queue create a dispatcher to handle message
			{
				return dispatcher(&q);
			}
		};
	} // namespace csp
} // namespace rola

#endif // !ROLA_CSP_RECEIVER_HPP
