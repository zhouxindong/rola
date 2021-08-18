#include <iostream>
#include "socket_init.hpp"
#include "socket2.hpp"
#include "connected_socket.hpp"
#include "tcp_server.hpp"
#include "tcp_client.hpp"
#include "udp_server.hpp"
#include "udp_client.hpp"

#include <thread>

static void accept_client(sockaddr_in peer, rola::connected_socket connsock)
{
	connsock.set_non_blocking(false);
	connsock.write("hello, world\n", 13);
	//bool read_timeout = connsock.set_read_timeout(std::chrono::seconds(3));
	//if (read_timeout)
	//	std::cout << "set read timeout successful\n";
	//else
	//	std::cout << "set read timeout failed\n";

	char buf[512];
	result_t len = connsock.read(buf, sizeof(buf));

	std::cout << "after read(): " << len << "\n";

	while (len == -1)
	{
		std::cout << "can't read data, retry...\n";
		std::this_thread::sleep_for(std::chrono::seconds(1));
		len = connsock.read(buf, sizeof(buf));
	}
	if (len > 0)
	{
		std::string s{ buf, buf + len };
		std::cout << s << "\n";
	}
	if (len == 0)
	{
		std::cout << "peer connect closed\n";
	}
}

int main_tcpserver()
{
	rola::socket_init sock_init;

	rola::tcp_server tser("192.168.168.1", 8899);

	std::thread t(&rola::tcp_server::start, &tser, accept_client, false);

	pause_wait_press();

	tser.stop();

	t.join();

	std::cout << "easy_socket2 tcp_server successful\n";
	return 0;
}

#include "tcp_client.hpp"

static void client_conn(sockaddr_in peer, rola::connected_socket connsock)
{
	//int rc;
	//char buf[120];

	//for (;;)
	//{
	//	rc = connsock.read(buf, sizeof(buf));
	//	if (rc == 1)
	//	{
	//		std::cout << "quit\n";
	//		break;
	//	}

	//	if (rc > 0)
	//	{
	//		std::string s;
	//		s.resize(rc);
	//		std::copy(buf, buf + rc, s.begin());

	//		std::cout << s << "\n";

	//		connsock.write("received\n", 8);
	//	}
	//}

	int rc;
	char buf[120];
	rc = connsock.read(buf, sizeof(buf));
	//std::cout << rola::detail::send_eof(connsock) << "\n";
	connsock.shutdown();

	std::this_thread::sleep_for(std::chrono::seconds(10));

}
int main_tcpclient()
{
	rola::socket_init sock_init;
	rola::tcp_client tcli("192.168.168.1", 8899);

	tcli.conn_proc(client_conn);

	std::cout << "easy_socket2 tcp_client successful\n";
	return 0;
}

static void udp_server_proc(rola::unconnected_socket sock)
{
	struct sockaddr_in peer;

	char buf[512];

	for (;;)
	{
		result_t rc = sock.read_from(buf, sizeof(buf), &peer);
		if (rc < 0)
			throw rola::socket_exception(rola::socket_exception_source::Recv);

		std::string s;
		s.resize(rc);
		std::copy(buf, buf + rc, s.begin());
		std::cout << s << "\n";

		if (sock.write_to("hello, world\n", 13, &peer) < 0)
			throw rola::socket_exception(rola::socket_exception_source::Send);
	}
}

int main_udpserver()
{
	rola::socket_init sock_init;

	rola::udp_server user("192.168.168.1", 8899);
	user.start(udp_server_proc);

	std::cout << "easy_socket2 udp_server successful\n";
	return 0;
}

static void udp_client_proc(rola::connected_socket sock)
{
	result_t rc;
	char buf[120];
	rc = sock.write("123456", 7);
	if (rc != 7)
		throw rola::socket_exception(rola::socket_exception_source::Send);

	rc = sock.read(buf, sizeof(buf));
	if (rc == -1)
		throw rola::socket_exception(rola::socket_exception_source::Recv);

	std::string s;
	s.resize(rc);
	std::copy(buf, buf + rc, s.begin());

	std::cout << s << "\n";
}
int main_udpclient()
{
	rola::socket_init sock_init;

		rola::udp_client ucli("192.168.168.1", 8899);

		ucli.conn_proc(udp_client_proc);



	std::cout << "easy_socket2 udp_client successful\n";
	return 0;
}


int main_udpsend()
{
	rola::socket_init sock_init;
	rola::udp_client ucli("192.168.168.1", 9000);
	int datagrams = 5000;
	char buf[1440];
	int rc;
	while (datagrams-- > 0)
	{
		rc = rola::detail::send(ucli.handle(), buf, 1440);
		if (rc <= 0)
		{
			std::cout << "send error: " << datagrams << "\n";
			break;
		}
	}

	pause_wait_press();

	return 0;
}

int main_udprecv()
{
	rola::socket_init sock_init;
	rola::udp_server usvr("192.168.168.1", 9000);
	int rc;
	int datagrams = 0;
	int rcvbufsz = 5000 * 1440;
	char buf[1440];
	usvr.set_option(SOL_SOCKET, SO_RCVBUF, rcvbufsz);
	for (;;)
	{
		rc = rola::detail::recv(usvr.handle(), buf, 1440);
		if (rc <= 0)
			break;
		datagrams++;
	}
	std::cout << "recv count: " << datagrams;

	pause_wait_press();
	return 0;
}

static void server2(sockaddr_in peer, rola::connected_socket connsock)
{
	connsock.write("hello, world\n", 13);
	bool read_timeout = connsock.set_read_timeout(std::chrono::seconds(3));
	if (read_timeout)
		std::cout << "set read timeout successful\n";
	else
		std::cout << "set read timeout failed\n";

	char buf[512];
	result_t len = connsock.read(buf, sizeof(buf));

	std::cout << "after read(): " << len << "\n";

	while (len == -1)
	{
		std::cout << "can't read data, retry...\n";
		std::this_thread::sleep_for(std::chrono::seconds(1));
		len = connsock.read(buf, sizeof(buf));
	}
	if (len > 0)
	{
		std::string s{ buf, buf + len };
		std::cout << s << "\n";
	}
}
