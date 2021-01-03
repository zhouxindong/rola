#include "socket.hpp"
#include "inet_address.hpp"
#include "datagram_socket.hpp"
#include "udp_socket.h"
#include <cassert>
#include <thread>

template <typename UDPSOCK>
void run_echo(UDPSOCK sock)
{
	ssize_t n;
	char buf[512];

	typename UDPSOCK::addr_t src_addr;

	while ((n = sock.recv_from(buf, sizeof(buf), &src_addr)) > 0)
		sock.send_to(buf, n, src_addr);
}

int main_usm()
{
	// 1. udp server
	rola::socket_initializer sockInit;
	rola::udp_socket sock;
	assert(sock);
	bool binded = sock.bind(rola::inet_address("127.0.0.1", 1234));
	assert(binded);

	std::thread thr(run_echo<rola::udp_socket>, std::move(sock));
	thr.join();

	// 2. udp client
	//rola::socket_initializer sockInit;
	//rola::udp_socket sock;

	//assert(sock.connect(rola::inet_address("127.0.0.1", 8080)));
	//std::cout << "Created UDP socket at: " << sock.address() << "\n";

	//std::string s = "hello";
	//sock.send(s);

	//std::string sr;
	//sr.resize(512);
	//rola::ssize_t len = sock.recv(&sr[0], 512);
	//std::cout << "received: " << len << "\n";

	return 0;
}