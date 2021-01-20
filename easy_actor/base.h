#ifndef ROLA_EASY_ACTOR_BASE_H
#define ROLA_EASY_ACTOR_BASE_H

#include <stdint.h>
#include <iostream>
#include <cassert>

namespace rola
{
	namespace actor
	{
		struct Address
		{
			uint32_t framework;
			uint32_t page;
			uint32_t index;
		};
	} // namespace actor
} // namespace rola

#endif // !ROLA_EASY_ACTOR_BASE_H
