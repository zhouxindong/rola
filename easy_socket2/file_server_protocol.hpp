#ifndef ROLA_EASY_SOCKET2_FILE_SERVER_PROTOCOL_HPP
#define ROLA_EASY_SOCKET2_FILE_SERVER_PROTOCOL_HPP

#if defined(_WIN32)
#else
#include <pthread.h>
#endif

#include <vector>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>

#include "utils/easy_ini.hpp"
#include "stlex/string.hpp"

namespace rola
{
	namespace detail
	{
		static constexpr char UPDATE_SUCCESS = '1';
		static constexpr char UPDATE_FAIL = '0';

		static std::initializer_list<char>& file_ctrl_protocol_head()
		{
			static std::initializer_list<char> head{ 'Y', 'S', 'P', 'A' };
			return head;
		}

		static std::initializer_list<char>& file_ctrl_protocol_tail()
		{
			static std::initializer_list<char> tail{ 'H', 'J', 'Y', 'D' };
			return tail;
		}

		/// <summary>
		/// ysp main path: /PATH_ROOT/PATH_NFSROOT
		/// ysp update path: /PATH_ROOT/UPDATE3559|UPDATE3531
		/// ysp download path: /PATH_ROOT/UPDATEDOWN
		/// ysp_version.ini : nfsroot, update3559|3531
		/// </summary>
		static const std::string PATH_ROOT{ "tmpp" };
		static const std::string PATH_NFSROOT{ "nfsroot" };
		static const std::string PATH_UPDATE3559{ "update3559" };
		static const std::string PATH_UPDATE3531{ "update3531" };
	} // namespace detail

	namespace conf
	{
		// file server port
		static constexpr short SERVER_PORT_3559			= 3559;
		static constexpr short SERVER_PORT_3531			= 3531;

		// master host version broad addr and port
		static const char* VERSION_MASTER_ADDR			= "225.2.2.5";
		static constexpr short VERSION_MASTER_PORT		= 3616;

		// small host version broad addr and port
		static const char* VERSION_SMALL_ADDR			= "226.2.2.6";
		static constexpr short VERSION_SMALL_PORT		= 3619;

		// ysp_version.ini
		static const std::string YSP_VERSION_INI{ "ysp_version.ini" };
		static const char* SECTION_SYSTEM		= "system";
		static const char* VERSION_KEY			= "version";

		// cache.ini
		static const char* CACHE_INI			= "cache.ini";
		static const char* CACHE_SYSTEM			= "SYSTEM";
		static const char* CACHE_IP				= "ip";
		static const char* CACHE_PARENT_IP		= "ntpParentIP";

		static constexpr int VERSION_INTERVAL	= 5;
	} // namespace conf

	inline std::string path_update3559()
	{
		std::ostringstream oss;
		oss << '/' << detail::PATH_ROOT << '/' << detail::PATH_UPDATE3559;
		return oss.str();
	}

	inline std::string path_update3531()
	{
		std::ostringstream oss;
		oss << '/' << detail::PATH_ROOT << '/' << detail::PATH_UPDATE3531;
		return oss.str();
	}

	inline std::string path_version3559()
	{
		std::ostringstream oss;
		oss << '/' << detail::PATH_ROOT << '/' << detail::PATH_UPDATE3559 << '/' << conf::YSP_VERSION_INI;
		return oss.str();
	}

	inline std::string path_version3531()
	{
		std::ostringstream oss;
		oss << '/' << detail::PATH_ROOT << '/' << detail::PATH_UPDATE3531 << '/' << conf::YSP_VERSION_INI;
		return oss.str();
	}

	inline std::string path_version_nfsroot()
	{
		std::ostringstream oss;
		oss << '/' << detail::PATH_ROOT << '/' << detail::PATH_NFSROOT << '/' << conf::YSP_VERSION_INI;
		return oss.str();
	}

	inline std::string gen_file_protocol_packet(const std::string& msg)
	{
		std::string ret;
		ret.append(detail::file_ctrl_protocol_head().begin(), detail::file_ctrl_protocol_head().end());
		ret.append(msg);
		ret.append(detail::file_ctrl_protocol_tail().begin(), detail::file_ctrl_protocol_tail().end());

		return ret;
	}

	inline std::string gen_file_protocol_packet(char c)
	{
		std::string ret;
		ret.append(detail::file_ctrl_protocol_head().begin(), detail::file_ctrl_protocol_head().end());
		ret.append(1, c);
		ret.append(detail::file_ctrl_protocol_tail().begin(), detail::file_ctrl_protocol_tail().end());

		return ret;
	}
} // namespace rola

namespace rola
{
	namespace detail
	{
		inline std::string get_version(const std::string& filepath)
		{
			try
			{
				Easy_ini ini(filepath);
				if (ini.ParseError() == 0) // successful
				{
					return rola::stlex::trim(ini.Get(rola::conf::SECTION_SYSTEM, rola::conf::VERSION_KEY, "---"));
				}
				else
				{
					return "---";
				}
			}
			catch (...)
			{
				return "---";
			}
		}

		inline std::string get_version_nfs()
		{
			return get_version(rola::path_version_nfsroot());
		}

		inline std::string get_version3559()
		{
			return get_version(rola::path_version3559());
		}

		inline std::string get_version3531()
		{
			return get_version(rola::path_version3531());
		}
	} // namespace detail

	namespace detail
	{
		inline void cancel_thread(std::thread& t)
		{
			if (t.joinable())
			{
#if defined(_WIN32)
#else
				pthread_cancel(t.native_handle());
#endif				
				t.join();
			}
		}
	} // namespace detail
} // namespace rola

namespace rola
{
	enum class file_server_type
	{
		Master,
		Small
	};

	enum class file_client_type
	{
		Terminal3559,
		Terminal3531,
		SmallHost
	};
} // namespace rola

#endif // ROLA_EASY_SOCKET2_FILE_SERVER_PROTOCOL_HPP
