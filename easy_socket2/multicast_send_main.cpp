#include <iostream>
#include "socket_init.hpp"
#include "multicast_send.hpp"
#include <thread>

int main_mulsend()
{
	rola::socket_init sock_init;

	rola::multicast_send mul_send("224.1.2.3", 8899);

	int count = 0;
	while (true)
	{
		mul_send.write("hello", 6);
		std::cout << "count: " << ++count << "\n";
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	mul_send.write("hello", 6);

	std::cout << "multicast send successful\n";
	return 0;
}