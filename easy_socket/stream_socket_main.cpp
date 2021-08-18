#include "stream_socket.hpp"
#include <iostream>
#include <cassert>

int main_stream_socket()
{
	rola::socket_initializer init;

	rola::stream_socket sock;
	assert(!sock);
	assert(!sock.is_open());

	rola::stream_socket sock2(socket_t(3));
	assert(sock2);
	assert(sock2.is_open());

	std::cout << "stream_socket_main successful" << std::endl;
	return 0;
}