#include "etcp.hpp"

static void server(SOCKET s, struct sockaddr_in* localp)
{
	struct sockaddr_in peer;

#ifdef unix
	socklen_t peerlen;
#else
	int peerlen;
#endif
	char buf[512];

	for (;;)
	{
		peerlen = sizeof(peer);
		if (recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*)&peer, &peerlen) < 0)
			error(1, errno, "recvfrom failed");
		if (sendto(s, "hello, world\n", 13, 0, (struct sockaddr*)&peer, peerlen) < 0)
			error(1, errno, "sendto failed");
	}
}

int main_udpserver(int argc, char** argv)
{
	struct sockaddr_in local;
	char* hname;
	char* sname;
	SOCKET s;

	INIT();

	if (argc == 2)
	{
		hname = NULL;
		sname = argv[1];
	}
	else
	{
		hname = argv[1];
		sname = argv[2];
	}

	set_address(hname, sname, &local, "udp");
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (!isvalidsock(s))
		error(1, errno, "socket call failed");
	if (bind(s, (struct sockaddr*)&local, sizeof(local)))
		error(1, errno, "bind failed");

	server(s, &local);
	EXIT(0);
}