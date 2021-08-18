#include <iostream>
#include "tcp_file_server.hpp"
#include "socket_init.hpp"

int main()
{
	rola::socket_init sock_init;

	rola::tcp_file_server tser("192.168.168.1", 8080, ".");

	//std::thread t(&rola::tcp_file_server::start, &tser, [](sockaddr_in, rola::connected_socket) {}, true);


	//rola::tcp_server tser("192.168.168.1", 8080);

	//std::thread t(&rola::tcp_server::start, &tser, [](sockaddr_in, rola::connected_socket) {
	//	while (true)
	//	{
	//		std::this_thread::sleep_for(std::chrono::seconds(1));
	//	}
	//	}, false);

	pause_wait_press();

	//tser.stop();

	//t.join();

	std::cout << "tcp_file_server successful\n";
	return 0;
}