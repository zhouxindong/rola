#include <iostream>
#include "file_server_protocol.hpp"

int main_fileserverprotocol()
{
	std::string file_info = "/tmp/nfsroot/run/hosttask|128323|766";
	std::cout << file_info.size() << "\n";
	std::string fipack = rola::gen_file_protocol_packet(file_info);
	std::cout << "size: " << fipack.size() << ", content: " << fipack << "\n";

	std::cout << "file server protocol successful\n";
	return 0;
}