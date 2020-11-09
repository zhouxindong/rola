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
	// used for those log items that be filtered
	template <typename LogItem, typename LogFormatter>
	inline null_channel<LogItem, LogFormatter>& dummy_channel()
	{
		static null_channel<LogItem, LogFormatter> nucnl;
		return nucnl;
	}

	template <typename Channel>
	class filter_base
	{
	public:
		typedef Channel channel_type;
	};

	// level filter
	// equals or above log level can be passed
	template <typename Channel>
	class level_filter : public filter_base<Channel>
	{
	public:
		level_filter(Channel& channel, EasyLogLevel level = rola::EasyLogLevel::EASY_LOG_TRACE) noexcept
			: channel_(channel)
			, level_(level)
		{}

	public:		
		level_filter& operator()(typename Channel::log_item_type& item) noexcept
		{
			if (item.level() >= level_)
			{
				channel_ << item;
			}
			else
			{
				dummy_channel<
					typename Channel::log_item_type, 
					typename Channel::log_formatter_type>() << item;
			}
			return *this;
		}

	private:
		Channel& channel_;
		EasyLogLevel level_;		
	};

	// set filter
	// log level includes in set can be passed
	template <typename Channel>
	class set_filter : public filter_base<Channel>
	{
	public:
		set_filter(Channel& channel, std::vector<EasyLogLevel> v) noexcept
			: channel_(channel)
		{
			for (auto i : v)
			{
				set_.insert(i);
			}
		}

	public:
		set_filter& operator()(typename Channel::log_item_type& item) noexcept
		{
			if (set_.find(item.level()) != set_.end())
				channel_ << item;
			else
				dummy_channel<
				typename Channel::log_item_type,
				typename Channel::log_formatter_type>() << item;

			return *this;
		}
	private:
		Channel& channel_;
		//std::unordered_set<EasyLogLevel> set_;
		std::set<EasyLogLevel> set_;
	};
} // namespace rola

#endif // !ROLA_EASY_LOG_LOG_FILTER_HPP
