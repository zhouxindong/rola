#include <iostream>
#include "easy_socket2/tcp_connect.hpp"
#include "easy_socket2/tcp_server.hpp"
#include "easy_socket2/tcp_client.hpp"
#include "easy_socket2/socket_init.hpp"
#include "easy_socket2/stream_parser.hpp"

int main_tcpconnect()
{
	rola::socket_init sock_init;
	rola::tcp_server server("192.168.168.1", 8080);

	rola::tcp_send_connect<rola::fixed_len_stream<16>> conn([](std::vector<char> v) {
		std::cout << "received len = " << v.size() << "\n";
		
		});

	std::thread t(&rola::tcp_server::start, &server, conn, false);

	pause_wait_press();

	server.stop();
	t.join();

	std::cout << "tcp connect successful\n";
	return 0;
}