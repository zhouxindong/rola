#include <iostream>
#include <algorithm>

#include "etcp.hpp"

static void client(SOCKET s, struct sockaddr_in* peerp)
{
	int rc;
	char buf[120];

	for (;;)
	{
		rc = recv(s, buf, sizeof(buf), 0);
		if (rc <= 0)
			break;
		
		std::string s;
		s.resize(rc);
		std::copy(buf, buf + rc, s.begin());

		std::cout << s << "\n";
	}
}

int main_tcpclient(int argc, char* argv[])
{
	struct sockaddr_in peer;
	SOCKET s;
	INIT();

	set_address(argv[1], argv[2], &peer, "tcp");
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (!isvalidsock(s))
		error(1, errno, "socket call failed");
	if (connect(s, (struct sockaddr*)&peer, sizeof(peer)))
		error(1, errno, "connect failed");
	client(s, &peer);
	EXIT(0);

	std::cout << "tcpclient_skel.cpp successful\n";
	return 0;
}