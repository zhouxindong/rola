#ifndef ROLA_EASY_SOCKET2_TCP_FILE_CLIENT_HPP
#define ROLA_EASY_SOCKET2_TCP_FILE_CLIENT_HPP

#include <memory>
#include <thread>
#include <sstream>

#include "easy_socket2/socket2.hpp"
#include "easy_socket2/stream_parser.hpp"
#include "easy_log/gcc_compose_logger.hpp"
#include "easy_socket2/file_server_protocol.hpp"
#include "stlex/string.hpp"
#include "easy_socket2/socket_init.hpp"
#include "easy_socket2/multicast_recv.hpp"
#include "utils/easy_ini.hpp"

namespace rola
{
	namespace detail
	{
		// file client state interface
		class file_client_state
		{
		public:
			virtual ~file_client_state() {}
			virtual socket_t start(compose_logger_t& logger) = 0;
			virtual void stop() = 0;
		};

		// 1. state: client try to connect the server control socket
		class ctrl_connect_state : public file_client_state
		{
		public:
			ctrl_connect_state(const char* hname, short port)
			{
				socket2::set_address(hname, port, &peer_);
				sock_.create_socket(SOCK_STREAM);
			}

			ctrl_connect_state(const ctrl_connect_state&)				= delete;
			ctrl_connect_state(ctrl_connect_state&&)					= delete;
			ctrl_connect_state& operator=(const ctrl_connect_state&)	= delete;
			ctrl_connect_state& operator=(ctrl_connect_state&&)			= delete;

			~ctrl_connect_state()
			{
				stop();
			}

		public:
			socket_t start(compose_logger_t& logger) override
			{
				logger.log(LOG(TRACE) << "\n\nfile client is trying to connect control socket: "
					<< sockaddrin2str(peer_));

				while (running_)
				{
					if (sock_.connect(&peer_))
					{
						logger.log(LOG(TRACE) << "file client connected control socket successful");
						socket_t s = sock_.handle();
						sock_.handle() = INVALID_SOCKET;
						return s;
					}
					else
						std::this_thread::sleep_for(std::chrono::seconds(connect_interval_s));
				}
				return INVALID_SOCKET;
			}

			void stop() override
			{
				running_ = false;
			}

		private:
			socket2 sock_;
			sockaddr_in peer_;
			bool running_{ true };
			int connect_interval_s{ 5 };
		};

		// 2. state: client try to connect the server data socket
		class data_connect_state : public file_client_state
		{
		public:
			data_connect_state(const char* hname, short port)
			{
				socket2::set_address(hname, port + 1, &peer_);
				sock_.create_socket(SOCK_STREAM);
			}

			data_connect_state(const data_connect_state&)				= delete;
			data_connect_state(data_connect_state&&)					= delete;
			data_connect_state& operator=(const data_connect_state&)	= delete;
			data_connect_state& operator=(data_connect_state&&)			= delete;

		public:
			socket_t start(compose_logger_t& logger) override
			{
				logger.log(LOG(TRACE) << "\n\nfile client is trying to connect data socket: "
					<< sockaddrin2str(peer_));

				int count = 0;
				while (true)
				{
					if (sock_.connect(&peer_))
					{
						logger.log(LOG(TRACE) << "file client connected data socket successful");
						socket_t s = sock_.handle();
						sock_.handle() = INVALID_SOCKET;
						return s;
					}
					else
					{
						if (++count > connect_count_) // time out
						{
							logger.log(LOG(TRACE) << "file client connect to data socket time out");
							return INVALID_SOCKET;
						}
						else
							std::this_thread::sleep_for(std::chrono::seconds(1));
					}
				}
			}

			void stop() override
			{
			}

		private:
			socket2 sock_;
			sockaddr_in peer_;
			int connect_count_{ 10 };
		};

		// 3. state: file receive from server
		class file_recv_state : public file_client_state
		{
		public:
			file_recv_state(socket_t ctrl_sock, socket_t data_sock)
				: ctrl_sock_{ ctrl_sock }
				, data_sock_{ data_sock }
			{
				parser_.matched_signal().connect(&file_recv_state::ctrl_socket_matched, *this);
			}

			file_recv_state(const file_recv_state&)				= delete;
			file_recv_state(file_recv_state&&)					= delete;
			file_recv_state& operator=(const file_recv_state&)	= delete;
			file_recv_state& operator=(file_recv_state&&)		= delete;

		public:
			socket_t start(compose_logger_t& logger) override
			{
				logger_ = &logger;

				logger.log(LOG(TRACE) << "start file receive process...");

				char buf[4096];
				int recv_len = 0;

				while (true)
				{
					recv_len = detail::recv(ctrl_sock_.handle(), buf, 4096);
					if (recv_len > 0)
					{
						parser_.push(buf, recv_len);
					}
					if (recv_len == 0)
						break;
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}

				return INVALID_SOCKET;
			}

			void stop() override
			{
			}

			bool update_success() const
			{
				return update_success_;
			}

		private:
			socket2 ctrl_sock_;
			socket2 data_sock_;
			compose_logger_t* logger_;
			bool update_success_{ false };

		private:
			size_t recved_count_{ 0 };
			head_tail_stream<> parser_{ detail::file_ctrl_protocol_head(), detail::file_ctrl_protocol_tail() };
			void ctrl_socket_matched(std::vector<char>& finfo)
			{
				std::string s{ finfo.begin(), finfo.end() };
				logger_->log(LOG(TRACE) << "received control message: " << s);

				if (s.size() == 1)
					handle_ctrlinfo(s);
				else
					handle_fileinfo(s);
			}

			void handle_ctrlinfo(std::string& ctrlinfo)
			{
				if (ctrlinfo == std::string{ UPDATE_SUCCESS })
					update_success_ = true;
			}

			void handle_fileinfo(std::string& fileinfo)
			{
				auto tokens = rola::stlex::split(fileinfo, { '|' });
				if (tokens.size() != 3)
					return;

				std::string new_filepath = gen_recv_filepath(tokens[0]);
				size_t filesize = std::atoll(tokens[1].c_str());

				if (filesize == 0)
				{
					rola::create_file(new_filepath);
				}
				else
				{
					if (recv_file(new_filepath.c_str(), filesize, data_sock_.handle()))
					{
						logger_->log(LOG(TRACE) << "received [" << ++recved_count_ << "] file, name: "
							<< new_filepath << ", size: " << filesize << " successful");
					}
					else
					{
						path(new_filepath).remove_file();
						return;
					}
				}

#if defined(_WIN32)
#else
				std::ostringstream oss;
				oss << "chmod " << tokens[2] << " " << new_filepath;
				system(oss.str().c_str());
#endif
			}

			std::string gen_recv_filepath(const std::string& filepath)
			{
				rola::path p(filepath);
				//p.replace(1, PATH_UPDATEDOWN);
				auto parent_path = p.parent_path();
				if (!parent_path.exists())
					create_directories(parent_path);
				return p.str();
			}
		};
	} // namespace detail

	class tcp_file_client
	{
	public:
		tcp_file_client(const char* hname, file_client_type client_type)
			: hname_{ hname }
			, logger_{ com_logger("file_client") }
			, client_type_{ client_type }
		{
		}

		tcp_file_client(const tcp_file_client&)				= delete;
		tcp_file_client(tcp_file_client&&)					= delete;
		tcp_file_client& operator=(const tcp_file_client&)	= delete;
		tcp_file_client& operator=(tcp_file_client&&)		= delete;

	public:
		void start()
		{
			switch (client_type_)
			{
			case rola::file_client_type::Terminal3559:
				start_3559(true);
				break;

			case rola::file_client_type::Terminal3531:
				start_3531(true);
				break;

			case rola::file_client_type::SmallHost:
				start_3559(false);
				start_3531(false);
				break;

			default:
				break;
			}
		}

	private:
		void start_3559(bool del_download)
		{
			auto success = start_download(conf::SERVER_PORT_3559);
			if (success)
			{
				make_nfsroot_exists();
				copy_download(detail::PATH_UPDATE3559);
				if (del_download) remove_download(detail::PATH_UPDATE3559);
			}
			else
			{
				remove_download(detail::PATH_UPDATE3559);
			}
		}

		void start_3531(bool del_download)
		{
			auto success = start_download(conf::SERVER_PORT_3531);
			if (success)
			{
				if (del_download)
				{
					make_nfsroot_exists();
					copy_download(detail::PATH_UPDATE3531);
					remove_download(detail::PATH_UPDATE3531);
				}
			}
			else
			{
				remove_download(detail::PATH_UPDATE3531);
			}
		}

	private:
		bool start_download(short port)
		{
			logger_.log(LOG(TRACE) << "file client start download from " << port << "...");
			state_.reset(new detail::ctrl_connect_state(hname_.c_str(), port));
			auto ctrl_sock = state_->start(logger_);

			state_.reset(new detail::data_connect_state(hname_.c_str(), port));
			auto data_sock = state_->start(logger_);

			if (data_sock == INVALID_SOCKET)
			{
				socket2::close(ctrl_sock);
				return false;
			}
			else
			{
				state_.reset(new detail::file_recv_state(ctrl_sock, data_sock));
				state_->start(logger_);
				detail::file_recv_state* filerecv = dynamic_cast<detail::file_recv_state*>(state_.get());
				return filerecv != nullptr ? filerecv->update_success() : false;
			}
		}

	private:
		void make_nfsroot_exists()
		{
			system("cd");
			std::ostringstream oss;
			oss << '/' << detail::PATH_ROOT << '/' << detail::PATH_NFSROOT;
			rola::path path_nfs(oss.str());
			if (!path_nfs.exists())
				create_directories(path_nfs);
		}

		void copy_download(const std::string& src_path)
		{
			system("cd");
			std::ostringstream oss;
			oss << "cp -rf /" << detail::PATH_ROOT << '/' << src_path << "/* /"
				<< detail::PATH_ROOT << '/' << detail::PATH_NFSROOT;
			system(oss.str().c_str());
		}

		void remove_download(const std::string& down_path)
		{
			system("cd");
			std::ostringstream oss;
			oss << "rm -rf /" << detail::PATH_ROOT << '/' << down_path;
			system(oss.str().c_str());
		}

	private:
		std::string hname_;
		std::unique_ptr<detail::file_client_state> state_;
		compose_logger_t& logger_;
		file_client_type client_type_;
	};
} // namespace rola


#endif // ROLA_EASY_SOCKET2_TCP_FILE_CLIENT_HPP
