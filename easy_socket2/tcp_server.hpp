#ifndef ROLA_EASY_SOCKET2_TCP_SERVER_HPP
#define ROLA_EASY_SOCKET2_TCP_SERVER_HPP

#include <thread>
#include <chrono>
#include <functional>

#include "easy_socket2/socket2.hpp"
#include "easy_socket2/connected_socket.hpp"
#include "easy_socket2/tcp_connect.hpp"
#include "easy_signal/easy_signal.hpp"

namespace rola
{
	class tcp_server : public socket2
	{
	public:
		tcp_server(const char* hname, const char* sname)
		{
			socket2::set_address(hname, sname, &local_);
			pre_build();
		}

		tcp_server(const char* hname, short port)
		{
			socket2::set_address(hname, port, &local_);
			pre_build();
		}

		tcp_server(const char* sname)
		{
			socket2::set_address(nullptr, sname, &local_);
			pre_build();
		}

		tcp_server(short port)
		{
			socket2::set_address(nullptr, port, &local_);
			pre_build();
		}

		// used for before listen, set socket options, example SOL_SOCKET/SO_RCVBUF
		tcp_server(const char* hname, short port, std::function<void(tcp_server&)> f)
		{
			socket2::set_address(nullptr, port, &local_);
			pre_build(f); 
		}

		tcp_server(const tcp_server&)				= delete;
		tcp_server& operator=(const tcp_server&)	= delete;
		tcp_server(tcp_server&&)					= delete;
		tcp_server& operator=(tcp_server&&)			= delete;

	public:
		void start(std::function<void(sockaddr_in, connected_socket)> f, bool block)
		{
			set_non_blocking(!block);

			while (running_)
			{
				sockaddr_in peer;
				socklen_t peerlen = sizeof(peer);

				/*
				* if the server socket is non-blocking, then the client socket 
				* created by accept call also is non-blocking
				*/
				socket_t s = ::accept(handle(), (sockaddr*)&peer, &peerlen);
				
				if (s != INVALID_SOCKET)
				{
					accepted_handle(f, peer, s);
				}
				else
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(ACCEPT_WAIT));
				}					
			}
		}

		void start_signal(bool block)
		{
			set_non_blocking(!block);

			while (running_)
			{
				sockaddr_in peer;
				socklen_t peerlen = sizeof(peer);
				socket_t s = ::accept(handle(), (sockaddr*)&peer, &peerlen);
				if (s != INVALID_SOCKET)
					accept_signal_.emit_(peer, s);
				else
					std::this_thread::sleep_for(std::chrono::milliseconds(ACCEPT_WAIT));
			}
		}

		void stop()
		{
			running_ = false;
		}

		rola::Easy_signal<void(sockaddr_in, socket_t)>& accept_signal()
		{
			return accept_signal_;
		}

	private:
		virtual void accepted_handle(std::function<void(sockaddr_in, connected_socket)> f,
			sockaddr_in peer, socket_t s)
		{
			std::thread t(f, peer, connected_socket{ s });
			t.detach();
		}

	private:
		void pre_build()
		{
			create_socket(SOCK_STREAM);
			set_reuse();
			bindto(&local_);

			if (::listen(handle(), NLISTEN) != 0)
				throw socket_exception(socket_exception_source::Listen);
		}

		void pre_build(std::function<void(tcp_server&)> f)
		{
			create_socket(SOCK_STREAM);
			set_reuse();			
			f(*this);
			bindto(&local_);

			if (::listen(handle(), NLISTEN) != 0)
				throw socket_exception(socket_exception_source::Listen);
		}

	private:
		sockaddr_in local_;
		bool running_{ true };

		rola::Easy_signal<void(sockaddr_in, socket_t)> accept_signal_;
	};
} // namespace rola

#endif // ROLA_EASY_SOCKET2_TCP_SERVER_HPP
