#include <iostream>

#include <easy_socket2/socket_init.hpp>
#include <easy_socket2/tcp_file_client.hpp>

int main_tcpfileclient()
{
	rola::socket_init sock_init;
	rola::tcp_file_client client("192.168.168.128", rola::file_client_type::Terminal3559);
	client.start();

	return 0;
}