#include "socket.hpp"
#include "udp_socket.h"
#include <iostream>
#include <cassert>
#include <typeinfo>

int main_sock()
{
	// to_timeval
	timeval tv = rola::to_timeval(std::chrono::microseconds(500));
	assert(tv.tv_sec == 0);
	assert(tv.tv_usec == 500);

	tv = rola::to_timeval(std::chrono::microseconds(2500000));
	assert(tv.tv_sec == 2);
	assert(tv.tv_usec == 500000);

	// socket ctor
	rola::socket sock;
	assert(!sock);
	assert(!sock.is_open());
	assert(sock.handle() == INVALID_SOCKET);
	assert(sock.last_error() == 0);

	// handle ctor
	constexpr auto HANDLE = socket_t(3);
	rola::socket sock2(HANDLE);
	assert(sock2);
	//assert(sock.is_open());
	//assert(sock.handle() == HANDLE);
	assert(sock.last_error() == 0);

	// move ctor
	rola::socket m_sock(std::move(sock2));
	assert(m_sock);
	assert(!sock2);

	// an unopened socket should give an error
	rola::socket sock3;
	int reuse = 1;
	socklen_t len = sizeof(int);
	assert(!sock3.get_option(SOL_SOCKET, SO_REUSEADDR, &reuse, &len));
	assert(!sock3);
	assert(sock3.last_error() != 0);
	sock3.clear();
	assert(sock3.last_error() == 0);
	sock3.clear(42);
	assert(sock3.last_error() == 42);
	assert(!sock3);

	// clear error
	auto sock4 = rola::socket::create(AF_INET, SOCK_STREAM);
	assert(!sock4);
	sock4.clear(42);
	assert(!sock4);
	sock4.clear();
	assert(!sock4);

	// socket family
	rola::socket sock5;
	assert(sock5.family() == AF_UNSPEC);

	// unbound socket should have creation family
	rola::socket sock6 = rola::socket::create(AF_INET, SOCK_STREAM);

#if defined(_WIN32)
	assert(sock.family() == AF_UNSPEC);
#else
	assert(sock.family() == AF_INET);
#endif

	// bound socket should have same family as address to which it's bound
	auto sock7 = rola::socket::create(AF_INET, SOCK_STREAM);
	rola::inet_address addr(12293);
	assert(!sock7.set_option(SOL_SOCKET, SO_REUSEADDR, reuse));
	assert(!sock7.bind(addr));
	assert(sock7.family() != addr.family());


	std::cout << "socket_main successful\n";

	return 0;
}