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

int main()
{
	// 1. udp server
	//rola::socket_initializer sockInit;
	//rola::udp_socket sock;
	//assert(sock);
	//bool binded = sock.bind(rola::inet_address("127.0.0.1", 1234));
	//assert(binded);

	//std::thread thr(run_echo<rola::udp_socket>, std::move(sock));
	//thr.join();

	// 2. udp client
	//rola::socket_initializer sockInit;
	//rola::udp_socket sock;

	//assert(sock.connect(rola::inet_address("172.10.10.155", 8080)));
	//std::cout << "Created UDP socket at: " << sock.address() << "\n";

	//std::string s = "hello";
	//sock.send(s);

	//std::string sr;
	//sr.resize(512);
	//rola::ssize_t len = sock.recv(&sr[0], 512);
	//std::cout << "received: " << len << "\n";

	// 3. multi-case sender(224.1.2.3:8080)
	//rola::socket_initializer sockInit;
	//rola::udp_socket sock;
	//
	//auto multi_addr = rola::inet_address("224.1.2.3", 8080);
	//unsigned char ttl = 100;
	//sock.set_option(IPPROTO_IP, IP_MULTICAST_TTL, ttl);
	//sock.send_to("hello world", multi_addr);

	// 4. multi-case receiver(224.0.0.88:8888)
	rola::socket_initializer sockInit;
	rola::udp_socket sock;
	assert(sock);
	bool binded = sock.bind(rola::inet_address("172.10.10.155", 8888));
	assert(binded);

	int loop = 1;
	assert(sock.set_option(IPPROTO_IP, IP_MULTICAST_LOOP, loop));

	struct ip_mreq mreq;
	mreq.imr_multiaddr.S_un.S_addr = inet_addr("224.0.0.88");
	mreq.imr_interface.S_un.S_addr = inet_addr("172.10.10.155");
	assert(sock.set_option(IPPROTO_IP, IP_ADD_MEMBERSHIP, mreq));

	std::string sr;
	sr.resize(512);
	ssize_t len = sock.recv(&sr[0], 512);
	std::cout << "received: " << len << std::endl;

	assert(sock.set_option(IPPROTO_IP, IP_DROP_MEMBERSHIP, mreq));

	std::cout << "udp_socket_main success" << std::endl;
	return 0;
}