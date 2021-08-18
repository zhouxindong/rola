#ifndef ROLA_EASY_SOCKET2_FILE_SERVER_PROTOCOL_HPP
#define ROLA_EASY_SOCKET2_FILE_SERVER_PROTOCOL_HPP

#include <vector>

namespace rola
{
	namespace detail
	{
		static constexpr char DATA_SOCKET_LISTEN = '1';

		static std::vector<char>& file_ctrl_protocol_head()
		{
			static std::vector<char> head{ 49, 32, 68, 11 };
			return head;
		}

		static std::vector<char>& file_ctrl_protocol_tail()
		{
			static std::vector<char> tail{ 76, 58, 17, 29 };
			return tail;
		}
	} // namespace detail

	inline std::string gen_file_packet(const std::string& msg)
	{
		std::string ret;
		ret.resize(msg.size() + 8);
		ret.append(detail::file_ctrl_protocol_head().begin(), detail::file_ctrl_protocol_head().end());
		ret.append(msg);
		ret.append(detail::file_ctrl_protocol_tail().begin(), detail::file_ctrl_protocol_tail().end());

		return ret;
	}

	inline std::string gen_file_packet(char c)
	{
		std::string ret;
		ret.resize(9);
		ret.append(detail::file_ctrl_protocol_head().begin(), detail::file_ctrl_protocol_head().end());
		ret.append(1, c);
		ret.append(detail::file_ctrl_protocol_tail().begin(), detail::file_ctrl_protocol_tail().end());

		return ret;
	}
} // namespace rola

#endif // ROLA_EASY_SOCKET2_FILE_SERVER_PROTOCOL_HPP
