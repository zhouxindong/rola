#ifndef ROLA_EASY_LOG_BASIC_LOGGER_HPP
#define ROLA_EASY_LOG_BASIC_LOGGER_HPP

#include <string>
#include <mutex>
#include <map>

#include "log_item.hpp"
#include "log_formatter.hpp"
#include "log_channel.hpp"
#include "log_filter.hpp"

namespace rola
{
	// cross thread access lock
	template <typename T>
	struct global_mutex
	{
		static std::mutex g_locker;
	};

	template <typename T>
	std::mutex global_mutex<T>::g_locker;

	// basic logger
	template <
		typename Channel, 
		typename Filter = null_filter<typename Channel::log_item_type, 
									  typename Channel::log_formatter_type>>
	class basic_logger
	{
	public:
		typedef basic_logger<Channel, Filter>				self_type;
		typedef Channel										channel_type;
		typedef Filter										filter_type;
		typedef typename channel_type::log_item_type		log_item_type;
		typedef typename channel_type::log_formatter_type	log_formatter_type;

	public:
		explicit basic_logger(std::string const& name) noexcept
			: channel_(name)
		{}

		basic_logger(basic_logger const&)				= delete;
		basic_logger& operator=(basic_logger const&)	= delete;
		basic_logger(basic_logger&&)					= delete;
		basic_logger& operator=(basic_logger&&)			= delete;

	public:
		static self_type& get(std::string const& name) noexcept
		{
			std::lock_guard<std::mutex> lock(global_mutex<void>::g_locker);
			if (loggers_.find(name) == loggers_.end())
				loggers_[name] = std::make_shared<self_type>(name);
				
			return *(loggers_[name]);
		}

	public:
		self_type& log(log_item_type const& item) noexcept
		{
			std::lock_guard<std::mutex> lock(global_mutex<void>::g_locker);
			filter_(channel_, item);
			return *this;
		}

		self_type& operator<<(log_item_type const& item) noexcept
		{
			return log(item);
		}

	private:
		static std::map<std::string, std::shared_ptr<self_type>> loggers_;

	private:
		channel_type channel_;
		filter_type filter_;
	};

	template <typename Channel, typename Filter>
	std::map<std::string, std::shared_ptr<basic_logger<Channel, Filter>>>
		basic_logger<Channel, Filter>::loggers_;

	// console logger for all levels
	typedef basic_logger<
		console_channel<log_item, line_formatter<log_item>>,
		null_filter<log_item, line_formatter<log_item>>>
		console_logger;

	// file logger for all levels
	typedef basic_logger<
		file_channel<log_item, line_formatter<log_item>>,
		null_filter<log_item, line_formatter<log_item>>>
		file_logger;

	// udp logger for all levels
	typedef basic_logger<
		udp_channel<log_item, line_formatter<log_item>>,
		null_filter<log_item, line_formatter<log_item>>>
		udp_logger;

	// console logger for above info level
	typedef basic_logger<
		console_channel<log_item, line_formatter<log_item>>,
		level_filter<log_item, line_formatter<log_item>, rola::EasyLogLevel::EASY_LOG_INFO>>
		console_info_logger;

	// file logger for above info level
	typedef basic_logger<
		file_channel<log_item, line_formatter<log_item>>,
		level_filter<log_item, line_formatter<log_item>, rola::EasyLogLevel::EASY_LOG_INFO>>
		file_info_logger;

	typedef basic_logger<
		udp_channel<log_item, line_formatter<log_item>>,
		level_filter<log_item, line_formatter<log_item>, rola::EasyLogLevel::EASY_LOG_INFO>>
		udp_info_logger;

	typedef basic_logger<
		console_channel<log_item, debug_formatter<log_item>>,
		null_filter<log_item, debug_formatter<log_item>>>
		console_debug;

	typedef basic_logger<
		file_channel<log_item, debug_formatter<log_item>>,
		null_filter<log_item, debug_formatter<log_item>>>
		file_debug;

	typedef basic_logger<
		udp_channel<log_item, debug_formatter<log_item>>,
		null_filter<log_item, debug_formatter<log_item>>>
		udp_debug;
} // namespace rola

#endif // !ROLA_EASY_LOG_BASIC_LOGGER_HPP
