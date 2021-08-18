#include <iostream>
#include "easy_socket2/tcp_file_server.hpp"
#include "easy_socket2/socket_init.hpp"

int main_tcpfileserver()
{
	std::cout << "file server test....\n";

	rola::socket_init sock_init;

	rola::tcp_file_server server("192.168.168.128", 8080, rola::path_update3559());
	server.start();


	std::cout << "tcp_file_server successful\n";
	return 0;
}