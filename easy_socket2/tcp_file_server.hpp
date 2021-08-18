#ifndef ROLA_EASY_SOCKET2_TCP_FILE_SERVER_HPP
#define ROLA_EASY_SOCKET2_TCP_FILE_SERVER_HPP

#include <string>
#include <mutex>
#include <chrono>

#include "easy_socket2/socket2.hpp"
#include "easy_socket2/file_server_protocol.hpp"
#include "utils/path.hpp"

namespace rola
{
	namespace detail
	{
		// file server state interface
		class file_server_state
		{
		public:
			virtual ~file_server_state() = 0
			{
			}
		};

		// 1. state: waiting for control socket connect
		class ctrl_listen_state : public file_server_state
		{
		public:
			ctrl_listen_state(const char* hname, short port)
			{
				sockaddr_in addr;
				socket2::set_address(hname, port, &addr);
				sock_.create_socket(SOCK_STREAM);
				sock_.set_reuse();
				sock_.bindto(&addr);
				sock_.listen();
			}

			ctrl_listen_state(const ctrl_listen_state&)				= delete;
			ctrl_listen_state(ctrl_listen_state&&)					= delete;
			ctrl_listen_state& operator=(const ctrl_listen_state&)	= delete;
			ctrl_listen_state& operator=(ctrl_listen_state&&)		= delete;

			~ctrl_listen_state() override
			{
				stop();
			}

		public:
			std::pair<socket_t, sockaddr_in> start()
			{
				sock_.set_non_blocking();
				sockaddr_in peer;
				while (running_)
				{
					{
						std::lock_guard<std::mutex> lg(mutex_);
						socket_t s = sock_.accept(&peer);
						if (s != INVALID_SOCKET)
						{
							sock_.close();
							return std::make_pair(s, peer);
						}
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(ACCEPT_WAIT));
				}
			}

			void stop()
			{
				running_ = false;
			}

		private:
			socket2 sock_;
			bool running_{ true };
			std::mutex mutex_;
		};

		// 2. state: waiting for data socket connect
		class data_listen_state : public file_server_state
		{
		public:
			data_listen_state(const char* hname, short port, socket_t ctrl_sock)
				: ctrl_sock_(ctrl_sock)
			{
				sockaddr_in addr;
				socket2::set_address(hname, port + 1, &addr);
				sock_.create_socket(SOCK_STREAM);
				sock_.set_reuse();
				sock_.bindto(&addr);
				sock_.listen();
			}

			data_listen_state(const data_listen_state&)				= delete;
			data_listen_state(data_listen_state&&)					= delete;
			data_listen_state& operator=(const data_listen_state&)	= delete;
			data_listen_state& operator=(data_listen_state&&)		= delete;

			~data_listen_state() override
			{
				stop();
			}

		public:
			std::pair<socket_t, sockaddr_in> start()
			{
				sock_.set_non_blocking();

				auto send_len = detail::send(ctrl_sock_, gen_file_packet(detail::DATA_SOCKET_LISTEN));
				if (send_len <= 0)
				{
					return std::make_pair(INVALID_SOCKET, sockaddr_in());
				}

				auto pt = std::chrono::steady_clock::now();

				sockaddr_in peer;
				while (running_)
				{
					socket_t s = sock_.accept(&peer);
					if (s != INVALID_SOCKET)
					{
						sock_.close();
						return std::make_pair(s, peer);
					}

					std::this_thread::sleep_for(std::chrono::milliseconds(ACCEPT_WAIT));
					auto pt2 = std::chrono::steady_clock::now();
					auto delay = std::chrono::duration_cast<std::chrono::seconds>(pt2 - pt);
					if (delay.count() > time_out_s_) // wait for connect time out
					{
						return std::make_pair(INVALID_SOCKET, sockaddr_in());
					}
				}
			}

			void stop()
			{
				running_ = false;
			}

		private:
			socket2 sock_;
			bool running_{ true };
			socket_t ctrl_sock_;
			int time_out_s_{ 10 };
		};

		// 3. state: file transfer from server to client
		class file_transfer_state : public file_server_state
		{
		public:
			file_transfer_state(
				socket_t ctrl_sock,
				socket_t data_sock,
				sockaddr_in ctrl_peer,
				sockaddr_in data_peer,
				std::vector<std::string>& files
			)
				: ctrl_sock_(ctrl_sock)
				, data_sock_(data_sock)
				, ctrl_peer_(ctrl_peer)
				, data_peer_(data_peer)
				, files_(files)
			{
			}

			file_transfer_state(const file_transfer_state&)				= delete;
			file_transfer_state(file_transfer_state&&)					= delete;
			file_transfer_state& operator=(const file_transfer_state&)	= delete;
			file_transfer_state& operator=(file_transfer_state&&)		= delete;

		public:
			void start()
			{
				
			}

		private:
			void send_file(const std::string& file_path)
			{

			}

		private:
			socket_t ctrl_sock_;
			socket_t data_sock_;
			sockaddr_in ctrl_peer_;
			sockaddr_in data_peer_;

			std::vector<std::string> files_;
		};

	} // namespace detail


	class tcp_file_server : public socket2
	{
	public:
		tcp_file_server(const char* hname, short port, const std::string& path)
			: path_(path)
			, ip_{hname}
			, port_{port}
		{
			path_valid_ = rola::path_files(path.c_str(), files_);
			if (!path_valid_)
				return;

			socket2::set_address(hname, port, &ctrl_addr_);
		}		

		tcp_file_server(const tcp_file_server&)				= delete;
		tcp_file_server(tcp_file_server&&)					= delete;
		tcp_file_server& operator=(const tcp_file_server&)	= delete;
		tcp_file_server& operator=(tcp_file_server&&)		= delete;

	public:
		bool path_valid() const
		{
			return path_valid_;
		}

		void start()
		{
			running_ = true;
			create_ctrl_socket();
		}

		void stop()
		{
			running_ = false;
		}

	private:
		// ctrl socket used base class socket2's data member socket_t
		void create_ctrl_socket()
		{
			create_socket(SOCK_STREAM);
			set_reuse();
			bindto(&ctrl_addr_);

			if (::listen(handle(), NLISTEN) != 0)
				throw socket_exception(socket_exception_source::Listen);			

			sockaddr_in peer;
			socklen_t peerlen = sizeof(peer);
			socket_t ctrl_sock;
			set_non_blocking();

			while (running_)
			{
				{
					std::lock_guard<std::mutex> lg(mutex_);
					ctrl_sock = ::accept(handle(), (sockaddr*)&peer, &peerlen);
					if (ctrl_sock != INVALID_SOCKET)
					{
						close();
						break;
					}
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(ACCEPT_WAIT));
			}

			create_data_socket(ctrl_sock);
		}

		void create_data_socket(socket_t ctrl_sock)
		{
			socket2 data_listen;

			socket2::set_address(ip_.c_str(), port_ + 1, &data_addr_);
			data_listen.create_socket(SOCK_STREAM);
			data_listen.set_reuse();
			data_listen.bindto(&data_addr_);

			if (::listen(data_listen.handle(), 1) != 0)
				throw socket_exception(socket_exception_source::Listen);
			data_listen.set_non_blocking();

			// notify client connect that data socket is listening
			auto send_len = detail::send(ctrl_sock, gen_file_packet(detail::DATA_SOCKET_LISTEN));
			if (send_len <= 0)
			{
				socket2::close(ctrl_sock);
				return;
			}

			socket_t data_sock;

			while (running_)
			{
				sockaddr_in peer_data;
				socklen_t peerlen = sizeof(peer_data);

				data_sock = ::accept(data_listen.handle(), (sockaddr*)&peer_data, &peerlen);

				if (data_sock != INVALID_SOCKET)
				{
					data_listen.close();
					break;
				}
				else
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(ACCEPT_WAIT));
				}
			}

			file_send(ctrl_sock, data_sock);
		}

		void file_send(socket_t ctrl_sock, socket_t data_sock)
		{
			for (const std::string& file : files_)
			{
				if (!running_)
				{
					socket2::close(ctrl_sock);
					socket2::close(data_sock);
					return;
				}


			}
		}

	private:
		std::string path_;
		std::vector<std::string> files_;
		bool path_valid_;

	private:
		std::string ip_;
		short port_;
		sockaddr_in ctrl_addr_;
		sockaddr_in data_addr_;
		bool running_{ true };
		std::mutex mutex_;

	};
} // namespace rola

#endif // ROLA_EASY_SOCKET2_TCP_FILE_SERVER_HPP
