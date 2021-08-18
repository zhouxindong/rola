#include <iostream>
#include <algorithm>
#include <string>
#include "etcp.hpp"

int main_tcpservervrec(int argc, char** argv)
{
    struct sockaddr_in peer;
    SOCKET s;
    SOCKET s1;
#ifdef unix
    socklen_t peerlen = sizeof(peer);
#else
    int peerlen = sizeof(peer);
#endif
    int n;
    char buf[10];

    INIT();
    if (argc == 2)
        s = tcp_server(NULL, argv[1]);
    else
        s = tcp_server(argv[1], argv[2]);
    s1 = accept(s, (struct sockaddr*)&peer, &peerlen);
    if (!isvalidsock(s1))
        error(1, errno, "accept failed");

    for (;;)
    {
        n = readvrec(s1, buf, sizeof(buf));
        if (n < 0)
            error(0, errno, "readvrec returned error");
        else if (n == 0)
            error(1, 0, "client disconnected\n");
        else
        {
            std::string s;
            s.resize(n);
            std::copy(buf, buf + n, s.begin());

            std::cout << s << "\n";
        }
    }
    EXIT(0);
}