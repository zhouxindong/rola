#ifndef ROLA_EASY_LOG_COMPOSE_LOGGER_HPP
#define ROLA_EASY_LOG_COMPOSE_LOGGER_HPP

#include <memory>
#include <utility>

#include "log_channel.hpp"
#include "log_filter.hpp"

namespace rola
{
#pragma region logger_item

	// base class for a logger used in compose logger
	template <typename LogItem, typename LogFormatter>
	class logger_item_base
	{
	public:
		using log_item_type			= LogItem;
		using log_formatter_type	= LogFormatter;

	public:
		virtual ~logger_item_base() noexcept {}

	public:
		virtual logger_item_base& log(log_item_type const& item) noexcept
		{
			(*filter_.get())(*channel_.get(), item);
			return *this;
		}

	protected:
		std::shared_ptr<channel_base<LogItem, LogFormatter>> channel_;
		std::shared_ptr<filter_base<LogItem, LogFormatter>> filter_;
	};

	// a console logger can be input to a compose logger
	template <typename LogItem, typename LogFormatter, typename Filter>
	class console_logger_item : public logger_item_base<LogItem, LogFormatter>
	{
	public:
		console_logger_item()
		{
			this->filter_ = std::make_shared<Filter>();
			this->channel_ = std::make_shared<console_channel<LogItem, LogFormatter>>();
		}
	};

	// a file logger can be input to a compose logger
	template <typename LogItem, typename LogFormatter, typename Filter, size_t FileMaxSize = 10 * 1000 * 1024>
	class file_logger_item : public logger_item_base<LogItem, LogFormatter>
	{
	public:
		template <typename ...Args>
		file_logger_item(Args&&... args)
		{
			this->filter_ = std::make_shared<Filter>();
			this->channel_ = std::make_shared<file_channel<LogItem, LogFormatter, FileMaxSize>>(
				std::forward<Args>(args)...);
		}
	};

	template <typename LogItem, typename LogFormatter, typename Filter>
	class udp_logger_item : public logger_item_base<LogItem, LogFormatter>
	{
	public:
		template <typename ... Ts>
		udp_logger_item(Ts&& ... ts)
		{
			this->filter_ = std::make_shared<Filter>();
			this->channel_ = std::make_shared<udp_channel<LogItem, LogFormatter>>(
				std::forward<Ts>(ts)...);
		}
	};

#pragma endregion

#pragma region compose_logger

	template <typename LogItem, typename LogFormatter>
	class compose_logger
	{
	public:
		template <
			//typename Filter, 
			typename I, 
			typename ...Args>
			void add(Args&&... args)
		{
			loggers_.push_back(std::make_shared<I>(std::forward<Args>(args)...));
		}

		void log(LogItem const& item)
		{
			for (auto const& logger : loggers_)
			{
				logger->log(item);
			}
		}

		compose_logger& operator<<(LogItem const& item)
		{
			log(item);
			return *this;
		}

	private:
		std::vector<std::shared_ptr<logger_item_base<LogItem, LogFormatter>>> loggers_;
	};

#pragma endregion
} // namespace rola

#endif // !ROLA_EASY_LOG_COMPOSE_LOGGER_HPP
