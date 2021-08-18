#include <iostream>
#include "broadcast_recv.hpp"
#include "socket_init.hpp"

int main_broadcastrecv()
{
	rola::socket_init sock_init;
	rola::broadcast_recv brecv(12345);

	char buf[128];
	int str_len;
	int count = 0;
	while (true)
	{
		str_len = brecv.read(buf, 128);
		if (str_len < 0)
			break;
		buf[str_len] = 0;
		std::cout << "count: " << ++count << ", message: " << buf << "\n";
	}
	std::cout << "broadcast recv successful\n";
	return 0;
}