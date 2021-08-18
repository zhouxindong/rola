#include "etcp.hpp"

int main_udpclientperf(int argc, char** argv)
{
	struct sockaddr_in peer;
	SOCKET s;
	int rc;
	int datagrams; // amount
	int dgramsz = 1440; // size
	char buf[1440]; // close to MTU

	INIT();
	datagrams = atoi(argv[2]);
	if (argc > 3)
		dgramsz = atoi(argv[3]);

	s = udp_client(argv[1], "9000", &peer);
	while (datagrams-- > 0)
	{
		rc = sendto(s, buf, dgramsz, 0,
			(struct sockaddr*)&peer, sizeof(peer));
		if (rc <= 0)
			error(0, errno, "sendto failed");
	}

	sendto(s, "", 0, 0, (struct sockaddr*)&peer, sizeof(peer)); // 0 byte used as EOF
	EXIT(0);
}