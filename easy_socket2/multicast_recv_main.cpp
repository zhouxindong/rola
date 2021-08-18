#include <iostream>
#include "multicast_recv.hpp"
#include "socket_init.hpp"

int main()
{
	rola::socket_init sock_init;
	rola::multicast_recv mul_recv("224.1.2.3", 8899);

	
	char buf[128];
	int str_len;
	int count = 0;
	while (true)
	{
		str_len = mul_recv.read(buf, 128);
		if (str_len < 0)
			break;
		buf[str_len] = 0;
		std::cout << "count: " << ++count << ", message: " << buf << "\n";
	}
	std::cout << "multicast recv successful\n";
	return 0;
}