#ifndef ROLA_EASY_LOG_LOG_FILTER_HPP
#define ROLA_EASY_LOG_LOG_FILTER_HPP

#include <memory>
//#include <unordered_set>
#include <set>
#include <vector>

#include "log_level.hpp"
#include "log_channel.hpp"

namespace rola
{
	// filter base class
	template <typename LogItem, typename LogFormatter>
	class filter_base
	{
	public:
		virtual ~filter_base() {}

	public:
		virtual filter_base& operator()(channel_base<LogItem, LogFormatter>& channel, LogItem const& item) noexcept = 0;
	};

	// null filter
	// no filte
	template <typename LogItem, typename LogFormatter>
	class null_filter : public filter_base<LogItem, LogFormatter>
	{
	public:
		null_filter& operator()(channel_base<LogItem, LogFormatter>& channel, LogItem const& item) noexcept override
		{
			channel << item;
			return *this;
		}
	};

	// level filter
	// equals or above log level can be passed
	template <typename LogItem, typename LogFormatter,
		rola::EasyLogLevel Level = rola::EasyLogLevel::EASY_LOG_TRACE>
	class level_filter : public filter_base<LogItem, LogFormatter>
	{
	public:		
		level_filter& operator()(channel_base<LogItem, LogFormatter>& channel, LogItem const& item) noexcept override
		{
			if (item.level() >= Level)
			{
				channel << item;
			}
			return *this;
		}
	};

	// set filter
	// log level includes in set can be passed
	template <typename LogItem, typename LogFormatter,
		rola::EasyLogLevel ...Levels>
	class set_filter : public filter_base<LogItem, LogFormatter>
	{
	public:
		set_filter() noexcept
		{
			std::initializer_list<rola::EasyLogLevel> v{ Levels... };
			for (auto i : v)
			{
				set_.insert(i);
			}
		}

	public:
		set_filter& operator()(channel_base<LogItem, LogFormatter>& channel, LogItem const& item) noexcept override
		{
			if (set_.find(item.level()) != set_.end())
				channel << item;

			return *this;
		}
	private:
		std::set<EasyLogLevel> set_;
	};
} // namespace rola

#endif // !ROLA_EASY_LOG_LOG_FILTER_HPP
