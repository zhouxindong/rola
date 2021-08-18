#ifndef ROLA_EASY_LOG_LOG_CHANNEL_HPP
#define ROLA_EASY_LOG_LOG_CHANNEL_HPP

#include "easy_socket/udp_socket.h" // WIN32, must include first

#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>

//#include "log_item.hpp"
//#include "log_formatter.hpp"
#include "utils/path.hpp"
#include "stlex/chrono_ex.hpp"
#include "stlex/string.hpp"

namespace rola
{
#pragma region null_stream

	// null_stream
	// a dummy stream, swallow anything
	class null_stream : public std::ostream
	{
	public:
		null_stream() noexcept : std::ostream(0)
		{}
	};

#pragma endregion

#pragma region channel_base

	// the base class for channel
	template <typename LogItem, typename LogFormatter>
	class channel_base
	{
	public:
		typedef LogItem			log_item_type;
		typedef LogFormatter	log_formatter_type;
	public:
		virtual ~channel_base() noexcept {}

	public:
		virtual std::ostream& stream() noexcept		= 0;
		virtual std::ostream& flush() noexcept		= 0;
		virtual bool ready() noexcept				= 0;

		virtual void log(log_item_type const& item, bool fls = true) noexcept
		{
			if (ready())
			{
				stream() << log_formatter_type::format(item);
				if (fls)
					flush();
			}
		}
	};

#pragma endregion

#pragma region null_channel

	// null_channel
	// used for those log items which be filtered
	template <typename LogItem, typename LogFormatter>
	class null_channel : public channel_base<LogItem, LogFormatter>
	{
	public:
		explicit null_channel(std::string const& = "") noexcept
		{}

		std::ostream& stream() noexcept override
		{
			return stream_;
		}

		std::ostream& flush() noexcept override
		{
			return stream_.flush();
		}

		bool ready() noexcept override
		{
			return true;
		}

		void log(LogItem const& item, bool flush = true) noexcept override
		{}
	private:
		null_stream stream_;
	};

#pragma endregion

#pragma region console_channel

	// console out channel
	template <typename LogItem, typename LogFormatter>
	class console_channel : public channel_base<LogItem, LogFormatter>
	{
	public:
		explicit console_channel(std::string const& = "") noexcept
		{}

		std::ostream& stream() noexcept override
		{
			return std::cerr;
		}

		std::ostream& flush() noexcept override
		{
			return std::cerr.flush();
		}

		bool ready() noexcept override
		{
			return true;
		}
	};

	template <typename LogItem, typename LogFormatter>
	inline console_channel<LogItem, LogFormatter>& terminal()
	{
		static console_channel<LogItem, LogFormatter> csl;
		return csl;
	}

#pragma endregion

#pragma region file_channel

	// get the file name based on date
	// in the same day, file size over max_size will create a new file
	inline std::string get_roll_name(
		std::string const& name,
		path const& pa,
		size_t max_size,
		int surfix = 1)
	{
		auto t = split_ymdhms(std::chrono::system_clock::now());
		std::ostringstream oss;
		oss << name << '_' << t.year
			<< std::setw(2) << std::setfill('0') << t.month
			<< std::setw(2) << std::setfill('0') << t.day;
		if (surfix != 1) oss << '_' << surfix;
		oss << ".log";

		path pb = pa / path(oss.str());
		if (pb.exists() && pb.file_size_noexcept() > max_size)
			return get_roll_name(name, pa, max_size, ++surfix);

		return oss.str();
	}

	static constexpr size_t LOG_FILE_MAX_SIZE = 10 * 1000 * 1024;

	// roll file limited a max size
	template <size_t FileMaxSize = LOG_FILE_MAX_SIZE>
	class roll_file
	{
		// make sure directory and sub-directory exists
		class ensure_directory_exist
		{
		public:
			ensure_directory_exist(path const& p) noexcept
				: path_(p)
			{
				create_directories(path_);
			}

			path const& get_path() const noexcept
			{
				return path_;
			}

		private:
			path path_;
		};

	public:
		roll_file(std::string const& log_path, std::string const& name) noexcept
			: dir_exist_{ path(log_path) }
			, today_{ rola::split_ymdhms(std::chrono::system_clock::now()).day }
			, name_{ name }
			, file_name_{ get_roll_name(name_, dir_exist_.get_path(), FileMaxSize) }
			, fstream_{ (dir_exist_.get_path() / path(file_name_)).str(), std::ios::app | std::ios::out }
		{}

		roll_file(roll_file const&)				= delete;
		roll_file& operator=(roll_file const&)	= delete;

		roll_file(roll_file&& rhs) noexcept
			: dir_exist_{ std::move(rhs.dir_exist_) }
			, today_{ rhs.today_ }
			, name_{ std::move(rhs.name_) }
			, file_name_{ std::move(rhs.file_name_) }
			, fstream_{ std::move(rhs.fstream_) }
		{}

		roll_file& operator=(roll_file&& rhs) noexcept
		{
			if (this != &rhs)
			{
				dir_exist_ = std::move(rhs.dir_exist_);
				today_ = rhs.today_;
				name_ = std::move(rhs.name_);
				file_name_ = std::move(rhs.file_name_);
				fstream_.flush();
				fstream_.close();
				fstream_ = std::move(rhs.fstream_);
			}
			return *this;
		}

		~roll_file() noexcept
		{
			if (fstream_)
				fstream_.close();
		}

	public:
		std::ofstream& stream() noexcept
		{
			path pb = dir_exist_.get_path() / file_name_;
			if (diff_day_() || pb.file_size_noexcept() > FileMaxSize)
			{
				fstream_.close();
				today_ = rola::split_ymdhms(std::chrono::system_clock::now()).day;
				file_name_ = get_roll_name(name_, dir_exist_.get_path(), FileMaxSize);
				fstream_ = std::ofstream((dir_exist_.get_path() / path(file_name_)).str(),
					std::ios::app | std::ios::out);
			}
			return fstream_;
		}

		std::ofstream& flush() noexcept
		{
			fstream_.flush();
			return fstream_;
		}

		bool ready() noexcept
		{
			return (bool)fstream_;
		}

	private:
		ensure_directory_exist dir_exist_;
		long today_;
		std::string name_;
		std::string file_name_;
		std::ofstream fstream_;

	private:
		bool diff_day_() noexcept
		{
			return split_ymdhms(std::chrono::system_clock::now()).day != today_;
		}
	};
	
	template <typename LogItem, typename LogFormatter,
		size_t FileMaxSize = LOG_FILE_MAX_SIZE>
	class file_channel : public channel_base<LogItem, LogFormatter>
	{
	public:
		explicit file_channel(std::string const& name, std::string const& path = "log") noexcept
			: roll_(path, name)
		{}

		file_channel(file_channel const&)				= delete;
		file_channel& operator=(file_channel const&)	= delete;

		file_channel(file_channel&& rhs)
			: roll_(std::move(rhs.roll_))
		{}

		file_channel& operator=(file_channel&& rhs)
		{
			if (this != &rhs)
			{
				roll_.stream().close();
				roll_ = std::move(rhs.roll_);
			}
			return *this;
		}

		std::ostream& stream() noexcept override
		{
			return roll_.stream();
		}

		std::ostream& flush() noexcept override
		{
			return roll_.flush();
		}

		bool ready() noexcept override
		{
			return roll_.ready();
		}
	private:
		roll_file<FileMaxSize> roll_;
	};

#pragma endregion

#pragma region udp_channel

	template <typename LogItem, typename LogFormatter>
	class udp_channel : public channel_base<LogItem, LogFormatter>
	{
	public:
		udp_channel(std::string const& addr)
		{
			auto tokens = stlex::split(addr, { ':', '@', '_' });
			if (tokens.size() < 2)
				return;

			in_port_t port = std::atoi(tokens[1].c_str());
			connected_ = sock_.connect(rola::inet_address(tokens[0], port));
		}

		std::ostream& stream() noexcept override
		{
			return oss_;
		}

		std::ostream& flush() noexcept override
		{
			return oss_.flush();
		}

		bool ready() noexcept override
		{
			return connected_;
		}

		void log(log_item_type const& item, bool fls = true) noexcept override
		{
			if (!ready())
				return;

			stream() << log_formatter_type::format(item);
			if (fls)
				flush();

			sock_.send(oss_.str());
			oss_.str("");
		}

	private:
		std::ostringstream oss_;

	private:
		rola::socket_initializer sock_init_;
		rola::udp_socket sock_;
		bool connected_{ false };
	};

#pragma endregion

	template <typename Channel>
	Channel& operator<<(Channel& out, typename Channel::log_item_type const& item)
	{
		out.log(item);
		return out;
	}

} // namespace rola
#endif // !ROLA_EASY_LOG_LOG_CHANNEL_HPP
