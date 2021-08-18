#ifndef ROLA_EASY_LOG_LOG_ITEM_HPP
#define ROLA_EASY_LOG_LOG_ITEM_HPP

#include <string>
#include <chrono>
#include <sstream>
#include <vector>

#include "stlex/chrono_ex.hpp"
#include "log_level.hpp"

namespace rola
{
	// represent a log item
	// 1. log_level
	// 2. emitted source file
	// 3. emitted function
	// 4. emitted source file lines
	// 5. emitted time
	// overload operator<< used for append any log message
	class log_item
	{
		friend std::ostream& operator<<(std::ostream& s, log_item const& item);
		//friend class line_formatter<log_item>;
		//friend class table_formatter<log_item>;

	public:
		log_item(EasyLogLevel level, std::string file, std::string caller, size_t line) noexcept
			: log_level_(level)
			, file_(file)
			, caller_(caller)
			, line_(line)
			, time_stamp_(std::chrono::system_clock::now())
		{}

		void swap(log_item& rhs) noexcept
		{
			std::swap(log_level_, rhs.log_level_);
			std::swap(file_, rhs.file_);
			std::swap(caller_, rhs.caller_);
			std::swap(line_, rhs.line_);
			std::swap(time_stamp_, rhs.time_stamp_);
			std::swap(stream_, rhs.stream_);
		}

		log_item(log_item const& rhs) noexcept
			: log_level_(rhs.log_level_)
			, file_(rhs.file_)
			, caller_(rhs.caller_)
			, line_(rhs.line_)
			, time_stamp_(rhs.time_stamp_)
			, stream_(rhs.stream_.str())
		{
			stream_.seekp(0, std::ios_base::end);
		}

		log_item(log_item&& rhs) noexcept
			: log_level_(rhs.log_level_)
			, file_(std::move(rhs.file_))
			, caller_(std::move(rhs.caller_))
			, line_(rhs.line_)
			, time_stamp_(std::move(rhs.time_stamp_))
			, stream_(std::move(rhs.stream_))
		{
		}

		log_item& operator=(log_item const& rhs) noexcept
		{
			log_item temp = rhs;
			swap(temp);
			stream_.seekp(0, std::ios_base::end);
			return *this;
		}

		log_item& operator=(log_item&& rhs) noexcept
		{
			swap(rhs);
			return *this;
		}

		virtual ~log_item() noexcept {};

	public:

		std::string message() const noexcept
		{
			return stream_.str();
		}

		EasyLogLevel level() const noexcept
		{
			return log_level_;
		}

		std::string const& file() const noexcept
		{
			return file_;
		}

		std::string const& caller() const noexcept
		{
			return caller_;
		}

		size_t line() const noexcept
		{
			return line_;
		}

		std::chrono::system_clock::time_point const& time_stamp() const noexcept
		{
			return time_stamp_;
		}

		template <typename T>
		log_item& operator<<(T const& data) noexcept
		{
			stream_ << data;
			return *this;
		}

	protected:
		virtual void print_out_(std::ostream& out) const noexcept
		{
			using namespace rola::chrono;

			out << "level:\t\t"		<< log_level_text(log_level_) << std::endl;
			out << "file:\t\t"		<< file_ << std::endl;
			out << "caller:\t\t"	<< caller_ << std::endl;
			out << "line:\t\t"		<< line_ << std::endl;
			out << "time:\t\t"		<< rola::split_ymdhms(time_stamp_) << std::endl;
			out << "message:\t"		<< message() << std::endl;
		}

	protected:
		EasyLogLevel log_level_;
		std::string file_;
		std::string caller_;
		size_t line_;
		std::chrono::system_clock::time_point time_stamp_;
		std::ostringstream stream_;
	};

	inline std::ostream& operator<<(std::ostream& s, log_item const& item)
	{
		item.print_out_(s);
		return s;
	}

#define LOG_TRACE		rola::log_item(rola::EasyLogLevel::EASY_LOG_TRACE,	__FILE__, __func__, __LINE__)
#define LOG_DEBUG		rola::log_item(rola::EasyLogLevel::EASY_LOG_DEBUG,	__FILE__, __func__, __LINE__)
#define LOG_INFO		rola::log_item(rola::EasyLogLevel::EASY_LOG_INFO,	__FILE__, __func__, __LINE__)
#define LOG_WARN		rola::log_item(rola::EasyLogLevel::EASY_LOG_WARN,	__FILE__, __func__, __LINE__)
#define LOG_ERROR		rola::log_item(rola::EasyLogLevel::EASY_LOG_ERROR,	__FILE__, __func__, __LINE__)
#define LOG_FATAL		rola::log_item(rola::EasyLogLevel::EASY_LOG_FATAL,	__FILE__, __func__, __LINE__)

#define LOG(level)		LOG_##level

	// generate test log items
	inline std::vector<log_item> generate_log_item(
		size_t count,
		std::string const& suffix = std::string()) noexcept
	{
		if (count == 0)
			return std::vector<log_item>();

		std::vector<log_item> v;
		v.reserve(count);

		for (size_t i = 0; i < count; ++i)
		{
			int r = rand() % 6;
			switch (r)
			{
			case 0:
				v.push_back(LOG(TRACE) << log_level_text(rola::EasyLogLevel::EASY_LOG_TRACE) << i << suffix);
				break;

			case 1:
				v.push_back(LOG(DEBUG) << log_level_text(rola::EasyLogLevel::EASY_LOG_DEBUG) << i << suffix);
				break;

			case 2:
				v.push_back(LOG(INFO) << log_level_text(rola::EasyLogLevel::EASY_LOG_INFO) << i << suffix);
				break;

			case 3:
				v.push_back(LOG(WARN) << log_level_text(rola::EasyLogLevel::EASY_LOG_WARN) << i << suffix);
				break;

			case 4:
				v.push_back(LOG(ERROR) << log_level_text(rola::EasyLogLevel::EASY_LOG_ERROR) << i << suffix);
				break;

			case 5:
				v.push_back(LOG(FATAL) << log_level_text(rola::EasyLogLevel::EASY_LOG_FATAL) << i << suffix);
				break;
			}
		}

		return v;
	}
} // namespace rola

#endif ROLA_EASY_LOG_LOG_ITEM_HPP