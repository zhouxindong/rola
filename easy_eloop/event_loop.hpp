#ifndef ROLA_EASY_ELOOP_EVENT_LOOP_HPP
#define ROLA_EASY_ELOOP_EVENT_LOOP_HPP

#include <utility>
#include <memory>

#include "event_listener.hpp"

namespace rola
{
	/*
	* The main EventLoop class
	* This is a singleton. A process should only have one event loop.
	*/
	class Event_loop
	{

	private:
		enum EventType
		{
			NONE,
			READ,
			WRITE,
			READ_WRITE,
			ERROR
		};

	};
} // namespace rola

#endif // !ROLA_EASY_ELOOP_EVENT_LOOP_HPP
