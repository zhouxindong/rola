#ifndef ROLA_EASY_LOG_LOG_LEVEL_HPP
#define ROLA_EASY_LOG_LOG_LEVEL_HPP

namespace rola
{
	// log level
	enum class EasyLogLevel
	{
		EASY_LOG_TRACE = 10155,
		EASY_LOG_DEBUG,
		EASY_LOG_INFO,
		EASY_LOG_WARN,
		EASY_LOG_ERROR,
		EASY_LOG_FATAL,
	};

	inline char const* log_level_text(rola::EasyLogLevel level)
	{
		switch (level)
		{
		case rola::EasyLogLevel::EASY_LOG_TRACE:
			return "TRACE";

		case rola::EasyLogLevel::EASY_LOG_DEBUG:
			return "DEBUG";

		case rola::EasyLogLevel::EASY_LOG_INFO:
			return "INFO";

		case rola::EasyLogLevel::EASY_LOG_WARN:
			return "WARN";

		case rola::EasyLogLevel::EASY_LOG_ERROR:
			return "ERROR";

		case rola::EasyLogLevel::EASY_LOG_FATAL:
			return "FATAL";

		default:
			return "UNKNOWN";
		}
	}
} // namespace rola

#endif // !ROLA_EASY_LOG_LOG_LEVEL_HPP
