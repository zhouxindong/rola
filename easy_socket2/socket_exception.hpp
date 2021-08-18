#ifndef ROLA_EASY_SOCKET2_SOCKET_EXCEPTION_HPP
#define ROLA_EASY_SOCKET2_SOCKET_EXCEPTION_HPP

#include <string>
#include <sstream>

#include "platform2.h"

// Used to explicitly ignore the returned value of a function call.
#define ignore_result(x) if (x) {}

namespace rola
{
	enum class socket_exception_source
	{
		Unknown,
		Init,
		Create,
		Bind,
		Listen,
		Accept,
		Connect,
		Close,
		Send,
		Recv,
		Destroy,
		Invalid,
		Address,
		Port,
		Reuse,
		Category,
		TTL,
		AddMembership,
		Broadcast,
	};

	inline std::string source_text(socket_exception_source src)
	{
		switch (src)
		{
		case socket_exception_source::Unknown:
			return "Unknown";

		case socket_exception_source::Init:
			return "Socket init";

		case socket_exception_source::Create:
			return "Socket create";

		case socket_exception_source::Bind:
			return "Socket bind";

		case socket_exception_source::Listen:
			return "Socket listen";

		case socket_exception_source::Accept:
			return "Socket accept";

		case socket_exception_source::Connect:
			return "Socket connect";

		case socket_exception_source::Close:
			return "Socket close";

		case socket_exception_source::Send:
			return "Socket send";

		case socket_exception_source::Recv:
			return "Socket recv";

		case socket_exception_source::Destroy:
			return "Socket destroy";

		case socket_exception_source::Invalid:
			return "Socket invalid";

		case socket_exception_source::Address:
			return "Socket address";

		case socket_exception_source::Port:
			return "Socket port";

		case socket_exception_source::Reuse:
			return "Socket set option for REUSE";

		case socket_exception_source::Category:
			return "Socket category mismatch";

		case socket_exception_source::TTL:
			return "Socket set TTL";

		case socket_exception_source::AddMembership:
			return "Socket add membership";

		case socket_exception_source::Broadcast:
			return "Socket broadcast";

		default:
			return "Non definition source";
		}
	}

	class socket_exception
	{
	public:
		socket_exception()
			: errno_{ errno }
			, src_{ socket_exception_source::Unknown }
		{
		}

		socket_exception(int err, socket_exception_source src)
			: errno_{ err }
			, src_{ src }
		{}

		explicit socket_exception(socket_exception_source src)
			: errno_{ -1 }
			, src_{ src }
		{}

		std::string message() const
		{
			if (errno_ == -1)
			{
				return std::string("Source: ") + source_text(src_);
			}

			char buf[1024];
			buf[0] = '\x0';

#if defined(_WIN32)
			::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, errno_, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				buf, sizeof(buf), NULL);
#else
#ifdef _GNU_SOURCE
			auto s = strerror_r(errno_, buf, sizeof(buf));
			return s ? std::string(s) : std::string();
#else
			ignore_result(strerror_r(errno_, buf, sizeof(buf)));
#endif
#endif
			std::ostringstream oss;
			oss << "Source: " << source_text(src_) << "\n" << "Message: " << buf;
			return oss.str();
			//return std::string(buf);
		}

	private:
		int errno_;
		socket_exception_source src_;
	};

	/// <summary>
	/// get error message with err
	/// </summary>
	/// <param name="err"></param>
	/// <returns></returns>
	inline std::string error_str(int err)
	{
		char buf[1024];
		buf[0] = '\x0';

#if defined(_WIN32)
		::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			buf, sizeof(buf), NULL);
#else
#ifdef _GNU_SOURCE
		auto s = strerror_r(err, buf, sizeof(buf));
		return s ? std::string(s) : std::string();
#else
		ignore_result(strerror_r(errno_, buf, sizeof(buf)));
#endif
#endif
		return std::string(buf);
	}
} // namespace rola

#endif // !ROLA_EASY_SOCKET2_SOCKET_EXCEPTION_HPP
