#ifndef ROLA_EASY_LOG_GCC_COMPOSE_LOGGER_HPP
#define ROLA_EASY_LOG_GCC_COMPOSE_LOGGER_HPP

#if defined(__GNUC__) && __GNUC__ < 5

#include <ostream>

namespace rola
{
	class log_item
	{};

	inline std:ostream & operator<<(std::ostream& s, log_item const& item)
	{
		return s;
	}

	template <typename LogItem = log_item>
	class line_formatter
	{};

	template <typename LogItem, typename LogFormatter>
	class compose_logger
	{
	public:
		void log(LogItem const& item)
		{}

		compose_logger& operator<<(LogItem const& item)
		{
			return *this;
		}
	};

	static compose_logger<log_item, line_formatter<log_item>>& com_logger(const char* log_file)
	{
		static compose_logger<log_item, line_formatter<log_item>> logger;
		return logger;
	}
} // namespace rola

#define LOG(level) rola::log_item()

#else

#include "easy_log/compose_logger.hpp"
#include "easy_log/log_item.hpp"
#include "easy_log/log_formatter.hpp"

namespace rola
{
	using all_log_level_filter	= level_filter<log_item, line_formatter<log_item>, rola::EasyLogLevel::EASY_LOG_TRACE>;
	using csl_logger_item		= console_logger_item<log_item, line_formatter<log_item>, all_log_level_filter>;
	using fi_logger_item		= file_logger_item<log_item, line_formatter<log_item>, all_log_level_filter>;

	static compose_logger<log_item, line_formatter<log_item>>& com_logger(const char* log_file)
	{
		static bool inited = false;
		static compose_logger<log_item, line_formatter<log_item>> logger;
		if (!inited)
		{
			logger.add<csl_logger_item>();
			logger.add<fi_logger_item>(log_file);
			inited = true;
		}
		return logger;
	}

} // namespace rola

#endif

namespace rola
{
	using compose_logger_t = compose_logger<log_item, line_formatter<log_item>>;
}

#endif // ROLA_EASY_LOG_GCC_COMPOSE_LOGGER_HPP
