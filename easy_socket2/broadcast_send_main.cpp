#include <iostream>
#include <thread>

#include "broadcast_send.hpp"
#include "socket_init.hpp"
#include "broadcast_send.hpp"

int main_broadcastsend()
{
	rola::socket_init sock_init;
	rola::broadcast_send broadsend(12345);

	int count = 0;
	while (true)
	{
		broadsend.write("hello", 6);
		std::cout << "count: " << ++count << "\n";
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	std::cout << "broadcast send successful\n";
	return 0;
}