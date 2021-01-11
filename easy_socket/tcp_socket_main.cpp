#include "tcp_socket.h"
#include "tcp_connector.h"
#include "tcp_acceptor.h"
#include <string>
#include <iostream>
#include <cassert>
#include <thread>

int main_tcpsocket()
{
	const std::string STR{ "This is a test. This is only a test." };
	const size_t N = STR.length();

	rola::socket_initializer init;
	rola::inet_address addr{ "localhost", 12345 };
	rola::tcp_acceptor asock{ addr };

	rola::tcp_connector csock;
	csock.set_non_blocking();
	assert(csock.connect(addr));

	auto ssock = asock.accept();
	assert(ssock);

	// read/write
	char buf[512];
	assert(csock.write_n(STR.data(), N) == N);
	assert(ssock.read_n(buf, N) == N);
	std::string str{ buf, buf + N };
	assert(str == STR);

	char buf2[512];
	assert(csock.write(STR) == N);
	assert(ssock.read_n(buf2, N) == N);
	std::string str2{ buf2, buf2 + N };
	assert(str2 == STR);

	// scatter/gather
	const std::string HEADER{ "<start>" },
		FOOTER{ "<end>" };

	const size_t N_HEADER = HEADER.length(),
		N_FOOTER = FOOTER.length(),
		N_TOT = N_HEADER + N + N_FOOTER;

	std::vector<iovec> outv{
		iovec{(void*)HEADER.data(), N_HEADER},
		iovec{(void*)STR.data(), N},
		iovec{(void*)FOOTER.data(), N_FOOTER}
	};

	char hbuf[512], fbuf[512];
	std::vector<iovec> inv{
		iovec{(void*)hbuf, N_HEADER},
		iovec{(void*)buf, N},
		iovec{(void*)fbuf, N_FOOTER}
	};

	assert(csock.write(outv) == N_TOT);
	assert(csock.write(outv) == N_TOT);
	assert(csock.write(outv) == N_TOT);

	assert(ssock.read(inv) == N_TOT);
	assert(std::string(hbuf, N_HEADER) == HEADER);
	assert(std::string(buf, N) == STR);
	assert(std::string(fbuf, N_FOOTER) == FOOTER);

	assert(ssock.read(inv) == N_TOT);
	assert(std::string(hbuf, N_HEADER) == HEADER);
	assert(std::string(buf, N) == STR);
	assert(std::string(fbuf, N_FOOTER) == FOOTER);

	assert(ssock.read(inv) == N_TOT);
	assert(std::string(hbuf, N_HEADER) == HEADER);
	assert(std::string(buf, N) == STR);
	assert(std::string(fbuf, N_FOOTER) == FOOTER);

	std::cout << "tcp_socket_main successful" << std::endl;
	return 0;
}

void run_echo(rola::tcp_socket sock)
{
	ssize_t n;
	char buf[512];
	while ((n = sock.read(buf, sizeof(buf))) > 0)
		sock.write_n(buf, n);

	std::cout << "connection closed from " << sock.peer_address() << std::endl;
}

int main_tcp_server()
{
	rola::socket_initializer init;
	rola::inet_address addr("172.10.10.155", 12345);
	rola::tcp_acceptor acc(addr);
	if (!acc)
	{
		std::cout << "Error creating the acceptor: " << acc.last_error_str() << std::endl;
		return -1;
	}
	std::cout << "listening at 172.10.10.155:12345..." << std::endl;
	while (true)
	{
		rola::inet_address peer;
		rola::tcp_socket sock = acc.accept(&peer);
		std::cout << "received a connection request from " << peer << std::endl;
		if (!sock)
		{
			std::cerr << "error accepting incoming connection: " << acc.last_error_str() << std::endl;
		}
		else
		{
			std::thread thr(run_echo, std::move(sock));
			thr.detach();
		}
	}

	std::cout << "tcp_socket_server successful" << std::endl;
	return 0;
}

int main_tcpclient()
{
	rola::socket_initializer init;
	rola::tcp_connector conn({ "172.10.10.155", 8080 });
	if (!conn)
	{
		std::cerr << "error connecting to server at " << rola::inet_address("172.10.10.155", 8080)
			<< std::endl << conn.last_error_str() << std::endl;
		return -1;
	}

	std::cout << "created a connection from " << conn.address() << std::endl;
	if (!conn.read_timeout(std::chrono::seconds(5)))
	{
		std::cerr << "error setting timeout on TCP stream: "
			<< conn.last_error_str() << std::endl;
	}

	char buf[1024];
	while (true)
	{
		ssize_t n = conn.read(buf, 1024);
		if (n > 0)
		{
			std::string s(buf, n);
			std::cout << s << std::endl;
		}
	}

	std::cout << "tcp_socket_client successful" << std::endl;
	return 0;
}