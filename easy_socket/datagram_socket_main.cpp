#include "datagram_socket.hpp"
#include "inet_address.hpp"
#include <string>
#include <iostream>
#include <cassert>

int main_datagram()
{
	rola::datagram_socket sock;
	assert(!sock);
	assert(!sock.is_open());

	rola::socket_initializer init;

	const auto ADDR = rola::inet_address("localhost", 12345);
	rola::datagram_socket sock2(ADDR);
	assert(sock2);
	assert(sock2.is_open());
	assert(sock2.last_error() == 0);
	assert(sock2.address() == ADDR);

	std::cout << "datagram_socket successful\n";
	return 0;
}