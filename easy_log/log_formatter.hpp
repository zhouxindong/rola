#ifndef ROLA_EASY_LOG_LOG_FORMATTER_HPP
#define ROLA_EASY_LOG_LOG_FORMATTER_HPP

#include <string>
#include <sstream>

namespace rola
{
	template <typename LogItem>
	class line_formatter
	{
	public:
		typedef LogItem log_item_type;
		typedef line_formatter<LogItem> type;

		static std::string format(log_item_type const& item) noexcept
		{
			std::ostringstream oss;
			oss << item.time_stamp() << ' ';
			oss << std::setw(5) << std::setfill(' ') << log_level_text(item.level()) << ' ';
			oss << '[' << item.file() << "]@" << item.caller() << '#' << item.line() << ": ";
			oss << item.message() << std::endl;

			return oss.str();
		}
	}; // class line_formatter

	template <typename LogItem, char Splitter = ';'>
	class table_formatter
	{
	public:
		typedef LogItem log_item_type;
		typedef table_formatter<LogItem> type;

		static std::string format(log_item_type const& item) noexcept
		{
			std::ostringstream oss;
			oss << item.time_stamp() << Splitter;
			oss << log_level_text(item.level()) << Splitter;
			oss << item.file() << Splitter;
			oss << item.caller() << Splitter;
			oss << item.line() << Splitter;
			oss << item.message() << std::endl;

			return oss.str();
		}
	};

	template <typename LogItem>
	class warn_detail_formatter
	{
	public:
		typedef LogItem log_item_type;
		typedef warn_detail_formatter<LogItem> type;

		static std::string format(log_item_type const& item) noexcept
		{
			std::ostringstream oss;
			oss << item.time_stamp() << ' ';
			if (static_cast<int>(item.level()) > static_cast<int>(rola::EasyLogLevel::EASY_LOG_INFO))
			{				
				oss << std::setw(5) << std::setfill(' ') << log_level_text(item.level()) << ' ';
				oss << '[' << item.file() << "]@" << item.caller() << '#' << item.line() << ": ";
			}
			oss << item.message() << std::endl;
			return oss.str();
		}
	};

	template <typename LogItem>
	class debug_formatter
	{
	public:
		typedef LogItem log_item_type;
		typedef debug_formatter<LogItem> type;

		static std::string format(log_item_type const& item) noexcept
		{
			std::ostringstream oss;
			oss << item.message() << ' ';
			return oss.str();
		}
	};

} // namespace rola

#endif // !ROLA_EASY_LOG_LOG_FORMATTER_HPP
