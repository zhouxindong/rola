#include "acceptor.hpp"
#include <iostream>
#include <cassert>

int main_acceptor()
{
	rola::acceptor sock;
	assert(!sock);
	assert(!sock.is_open());

	rola::acceptor sock2(socket_t(3));
	assert(sock2);
	assert(sock2.is_open());

	rola::socket_initializer init;
	const auto ADDR = rola::inet_address("localhost", 12345);
	rola::acceptor sock3(ADDR);
	assert(sock3);
	assert(sock3.is_open());
	assert(sock3.last_error() == 0);
	assert(sock3.address() == ADDR);

	std::cout << "acceptor_main successful" << std::endl;
	return 0;
}