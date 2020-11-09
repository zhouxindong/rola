#ifndef ROLA_EASY_LOG_BASIC_LOGGER_HPP
#define ROLA_EASY_LOG_BASIC_LOGGER_HPP

#include <string>

namespace rola
{
	template <typename Channel, typename Filter>
	class basic_logger
	{
	public:
		typedef basic_logger						self_type;
		typedef typename Channel							channel_type;
		typedef typename channel_type::log_item_type		log_item_type;
		typedef typename channel_type::log_formatter_type	log_formatter_type;

	private:
		std::string name_;

	};
} // namespace rola

#endif // !ROLA_EASY_LOG_BASIC_LOGGER_HPP
