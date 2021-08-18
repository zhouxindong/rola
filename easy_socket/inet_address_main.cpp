#include "inet_address.hpp"
#include <iostream>
#include <string>
#include <cassert>

const uint32_t ANY_ADDR{ INADDR_ANY };
const uint32_t LOCALHOST_ADDR{ INADDR_LOOPBACK };
const std::string LOCALHOST_STR{ "localhost" };
const in_port_t PORT{ 12345 };

int main_inetaddress()
{
	rola::inet_address addr;
	assert(!addr.is_set());
	assert(0 == addr.address());
	assert(0 == addr.port());
	assert(sizeof(sockaddr_in) == addr.size());

	// creating address from int32
	addr.create(LOCALHOST_ADDR, PORT);
	assert(addr.is_set());
	assert(LOCALHOST_ADDR == addr.address());
	assert(PORT == addr.port());

	assert(uint8_t((LOCALHOST_ADDR >> 0) & 0xFF) == addr[0]);
	assert(uint8_t((LOCALHOST_ADDR >> 8) & 0xFF) == addr[1]);
	assert(uint8_t((LOCALHOST_ADDR >> 16) & 0xFF) == addr[2]);
	assert(uint8_t((LOCALHOST_ADDR >> 24) & 0xFF) == addr[3]);

	assert(AF_INET == addr.sockaddr_in_ptr()->sin_family);
	assert(LOCALHOST_ADDR == ntohl(addr.sockaddr_in_ptr()->sin_addr.S_un.S_addr));
	assert(PORT == ntohs(addr.sockaddr_in_ptr()->sin_port));

	// only a port number this should use the "any" address 
	// to bind to all interfaces (typ for server
	rola::inet_address addr2(PORT);
	assert(addr2.is_set());
	assert(ANY_ADDR == addr2.address());
	assert(PORT == addr2.port());
	assert(AF_INET == addr.sockaddr_in_ptr()->sin_family);

	std::string ip1 = "192.168.1.1";
	std::string ip11 = rola::detail::valid_ip(ip1);
	std::cout << ip11 << "\n";
	assert(ip1 == ip11);

	std::string ip2 = "192.168.001.01";
	std::string ip21 = rola::detail::valid_ip(ip2);
	std::cout << ip21 << "\n";
	assert(ip21 == ip11);

	std::cout << "inet_address_main successful\n";
	return 0;
}