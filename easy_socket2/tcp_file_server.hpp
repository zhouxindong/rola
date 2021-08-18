#ifndef ROLA_EASY_SOCKET2_TCP_FILE_SERVER_HPP
#define ROLA_EASY_SOCKET2_TCP_FILE_SERVER_HPP

#include <string>
#include <mutex>
#include <chrono>
#include <memory>
#include <thread>
#include <utility>
#include <atomic>

#include "easy_socket2/socket2.hpp"
#include "easy_socket2/file_server_protocol.hpp"
#include "utils/path.hpp"
#include "easy_log/gcc_compose_logger.hpp"
#include "easy_socket2/socket_init.hpp"
#include "easy_socket2/multicast_send.hpp"

// file transfer server
namespace rola
{
	namespace detail
	{
		// file server state interface
		class file_server_state
		{
		public:
			virtual ~file_server_state()
			{
			}

			virtual std::pair<socket_t, sockaddr_in> start(compose_logger_t& logger) = 0;
			virtual void stop() = 0;
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
			std::pair<socket_t, sockaddr_in> start(compose_logger_t& logger) override
			{
				logger.log(LOG(TRACE) << "\n\nfile server control socket listen...");

				sock_.set_non_blocking();
				sockaddr_in peer;
				while (running_)
				{
					{
						std::lock_guard<std::mutex> lg(mutex_);
						socket_t s = sock_.accept(&peer);
						if (s != INVALID_SOCKET)
						{
							logger.log(LOG(TRACE) << "accepted client control connect from " << sockaddrin2str(peer));
							return std::make_pair(s, peer);
						}
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(ACCEPT_WAIT));
				}
				return std::make_pair(INVALID_SOCKET, sockaddr_in{});
			}

			void stop() override
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
			std::pair<socket_t, sockaddr_in> start(compose_logger_t& logger) override
			{
				logger.log(LOG(TRACE) << "file server data socket listening...");

				sock_.set_non_blocking();

				// wait for a while for client data connect
				auto pt = std::chrono::steady_clock::now();

				sockaddr_in peer;
				while (running_)
				{
					socket_t s = sock_.accept(&peer);
					if (s != INVALID_SOCKET)
					{
						logger.log(LOG(TRACE) << "accepted client data connect from " << sockaddrin2str(peer));
						sock_.close();
						return std::make_pair(s, peer);
					}

					std::this_thread::sleep_for(std::chrono::milliseconds(ACCEPT_WAIT));
					auto pt2 = std::chrono::steady_clock::now();
					auto delay = std::chrono::duration_cast<std::chrono::seconds>(pt2 - pt);
					if (delay.count() > time_out_s_) // wait for connect time out
					{
						logger.log(LOG(TRACE) << "data socket listening time out: " << time_out_s_ << " seconds");
						socket2::close(ctrl_sock_);
						return std::make_pair(INVALID_SOCKET, sockaddr_in());
					}
					else
						std::this_thread::sleep_for(std::chrono::milliseconds(ACCEPT_WAIT));
				}

				return std::make_pair(INVALID_SOCKET, sockaddr_in());
			}

			void stop() override
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
				const std::string path
			)
				: ctrl_sock_(ctrl_sock)
				, data_sock_(data_sock)
				, ctrl_peer_(ctrl_peer)
				, data_peer_(data_peer)
			{
				path_files(path.c_str(), files_);
				ctrl_sock_.set_non_blocking();
			}

			file_transfer_state(const file_transfer_state&)				= delete;
			file_transfer_state(file_transfer_state&&)					= delete;
			file_transfer_state& operator=(const file_transfer_state&)	= delete;
			file_transfer_state& operator=(file_transfer_state&&)		= delete;

		public:
			std::pair<socket_t, sockaddr_in> start(compose_logger_t& logger) override
			{
				for (size_t i = 0; i < files_.size(); ++i)
				{
					if (!process_file(i, logger))
					{
						detail::send_force(ctrl_sock_.handle(), gen_file_protocol_packet(UPDATE_FAIL));
						break;
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
				detail::send_force(ctrl_sock_.handle(), gen_file_protocol_packet(UPDATE_SUCCESS));
				return std::make_pair(INVALID_SOCKET, sockaddr_in{});
			}

			void stop() override
			{
			}

		private:
			bool process_file(size_t index, compose_logger_t& logger)
			{
				logger.log(LOG(TRACE) << "\n[" << index + 1 << "/" << files_.size() << "] " << files_[index]);

				auto finfo = build_file_info_str(files_[index]);
				logger.log(LOG(TRACE) << "file info: " << finfo.first);
				if (finfo.first.empty())
					return true;

				// send file info through ctrl socket
				if (!detail::send_force(ctrl_sock_.handle(), gen_file_protocol_packet(finfo.first)))
				{
					logger.log(LOG(ERROR) << "send file info error");
					return false;
				}

				if (finfo.second == 0) // file size is 0
					return true;

				// send file content through data socket
				size_t sentsize = send_file(files_[index].c_str(), data_sock_.handle());
				logger.log(LOG(TRACE) << "send file contents: " << sentsize << "\n");
				return sentsize == finfo.second;
			}

			static std::pair<std::string, size_t> get_ppp()
			{
				return std::make_pair(std::string{}, 0);
			}

			// file_path|file_size|file_mode
			static std::pair<std::string, size_t> build_file_info_str(const std::string& file_path, size_t fsize)
			{
				std::ostringstream oss;
				oss << file_path << '|' << fsize << '|' << rola::file_per_valstr(file_path.c_str());
				return std::make_pair(oss.str(), fsize);
			}

			static std::pair<std::string, size_t> build_file_info_str(const std::string& file_path)
			{
				rola::path p(file_path);
				if (!p.exists()) return std::make_pair(std::string{}, 0);
				if (!p.is_file()) return std::make_pair(std::string{}, 0);

				size_t fsize = p.file_size();
				return build_file_info_str(file_path, fsize);
			}

		private:
			socket2 ctrl_sock_;
			socket2 data_sock_;
			sockaddr_in ctrl_peer_;
			sockaddr_in data_peer_;

			std::vector<std::string> files_;
		};

	} // namespace detail

	// file server 
	class tcp_file_server
	{
	public:
		tcp_file_server(const char* hname, short port, const std::string& path)
			: hname_{ hname }
			, port_{ port }
			, path_{ path }
			, state_{ new detail::ctrl_listen_state(hname, port) }
			, logger_{ com_logger("file_server") }
		{
		}

		tcp_file_server(const tcp_file_server&)				= delete;
		tcp_file_server(tcp_file_server&&)					= delete;
		tcp_file_server& operator=(const tcp_file_server&)	= delete;
		tcp_file_server& operator=(tcp_file_server&&)		= delete;

	public:
		void start()
		{
			logger_.log(LOG(TRACE) << "file server start at: " << hname_ << ':' << port_ << ", update path: " << path_);

			while (true)
			{
			INIT:
				auto ctrl_client = state_->start(logger_);
				state_.reset(new detail::data_listen_state(hname_.c_str(), port_,
					ctrl_client.first));
				auto data_client = state_->start(logger_);
				if (data_client.first == INVALID_SOCKET)
				{
					state_.reset(new detail::ctrl_listen_state(hname_.c_str(), port_));
					goto INIT;
				}
				else
				{
					state_.reset(new detail::file_transfer_state(
						ctrl_client.first, data_client.first,
						ctrl_client.second, data_client.second, path_));
					state_->start(logger_);

					logger_.log(LOG(TRACE) << "finished file transfer to: " << sockaddrin2str(ctrl_client.second)
						<< ", " << sockaddrin2str(data_client.second));

					std::this_thread::sleep_for(std::chrono::seconds(1));
					state_.reset(new detail::ctrl_listen_state(hname_.c_str(), port_));
					goto INIT;
				}
			}
		}

	private:
		std::string hname_;
		short port_;
		std::string path_;
		std::unique_ptr<detail::file_server_state> state_;

	private:
		compose_logger<log_item, line_formatter<log_item>>& logger_;
	};
} // namespace rola

namespace rola
{
	/// <summary>
	/// updater server
	/// 1. broad version info
	/// 2. file server for 3559
	/// 3. file server for 3531
	/// </summary>
	class updater_server
	{
	public:
		updater_server(file_server_type server_type) noexcept(false)
			: version_send_(
				server_type == file_server_type::Master ? rola::conf::VERSION_MASTER_ADDR : rola::conf::VERSION_SMALL_ADDR
				, server_type == file_server_type::Master ? rola::conf::VERSION_MASTER_PORT : rola::conf::VERSION_SMALL_PORT)
			, logger_{ com_logger("file_server") }
		{
			logger_.log(LOG(TRACE) << "updater_server ctor start...");

			Easy_ini ini(rola::conf::CACHE_INI);
			if (ini.ParseError() == 0)
			{
				local_ip_ = ini.Get(rola::conf::CACHE_SYSTEM, rola::conf::CACHE_IP, std::string{});

				logger_.log(LOG(TRACE) << "parse " << rola::conf::CACHE_INI << " file successful, local ip: " << local_ip_);
			}

			if (!local_ip_.empty())
			{
				logger_.log(LOG(TRACE) << "ready to start version broadcast thread");
				broad_version_thread_ = std::thread([this]() {
					this->broad_version_thread_proc();
					});

				logger_.log(LOG(TRACE) << "ready to start file server for update 3559");
				file_server3559_thread_ = std::thread([this]() {
					tcp_file_server server(this->local_ip_.c_str(), rola::conf::SERVER_PORT_3559, rola::path_update3559());
					server.start();
					});

				logger_.log(LOG(TRACE) << "ready to start file server for update 3531");
				file_server3531_thread_ = std::thread([this]() {
					rola::tcp_file_server server(this->local_ip_.c_str(), rola::conf::SERVER_PORT_3531, rola::path_update3531());
					server.start();
					});
			}

			logger_.log(LOG(TRACE) << "updater_server ctor finished");
		}

		~updater_server() noexcept
		{
			if (broad_version_thread_.joinable()) broad_version_thread_.join();
			if (file_server3559_thread_.joinable()) file_server3559_thread_.join();
			if (file_server3531_thread_.joinable()) file_server3531_thread_.join();
		}

		void quit()
		{
			logger_.log(LOG(TRACE) << "ready to quit updater server");
			detail::cancel_thread(broad_version_thread_);
			detail::cancel_thread(file_server3559_thread_);
			detail::cancel_thread(file_server3531_thread_);
			logger_.log(LOG(TRACE) << "finished to quit updater server");
		}

		void enable_broad(bool val)
		{
			enable_.store(val);
		}

	private:
		void broad_version_thread_proc()
		{
			std::ostringstream oss;
			while (true)
			{
				if (enable_.load())
				{
					oss.str("");
					oss << local_ip_ << '|' << "3559:" << detail::get_version3559() << '|' << "3531:" << detail::get_version3531();
					version_send_.write(oss.str());
				}

				std::this_thread::sleep_for(std::chrono::seconds(rola::conf::VERSION_INTERVAL));
			}
		}

	private:
		std::string local_ip_{};
		std::thread broad_version_thread_;
		std::thread file_server3559_thread_;
		std::thread file_server3531_thread_;
		rola::multicast_send version_send_;

	private:
		rola::socket_init sock_init;
		rola::compose_logger_t& logger_;

	private:
		std::atomic<bool> enable_{ true };
	};
} // namespace rola

#endif // ROLA_EASY_SOCKET2_TCP_FILE_SERVER_HPP
