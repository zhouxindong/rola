#include <iostream>
#include <algorithm>
#include <string>

#include "etcp.hpp"

static void client(SOCKET s, struct sockaddr_in* peerp)
{
	int rc;
#ifdef unix
	socklen_t peerlen;
#else
	int peerlen;
#endif
	char buf[120];
	peerlen = sizeof(*peerp);
	if (sendto(s, "", 1, 0, (struct sockaddr*)peerp, peerlen) < 0)
		error(1, errno, "sendto failed");
	rc = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*)peerp, &peerlen);
	if (rc >= 0)
	{
		std::string s;
		s.resize(rc);
		std::copy(buf, buf + rc, s.begin());

		std::cout << s << "\n";
	}
	else
		error(1, errno, "recvfrom failed");
}

int main_udpclient(int argc, char** argv)
{
	struct sockaddr_in peer;
	SOCKET s;
	INIT();

	set_address(argv[1], argv[2], &peer, "udp");
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (!isvalidsock(s))
		error(1, errno, "socket call failed");
	client(s, &peer);
	exit(0);
}