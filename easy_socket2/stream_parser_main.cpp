#include <iostream>
#include "stream_parser.hpp"
#include <stdlib.h>
#include <thread>
#include <chrono>
#include <algorithm>
#include <iterator>

int main_streamparser()
{
	// 1. fixed_len_stream
	//rola::fixed_len_stream<16> fixedstream;
	//fixedstream.matched_signal().connect([](std::vector<char> v) {
	//	std::cout << "received: ";
	//	for (auto i : v)
	//	{
	//		std::cout << std::hex << (int)i << " ";
	//	}
	//	std::cout << "\n";
	//	});

	//while (true)
	//{
	//	char buf[10];
	//	buf[0] = 45;
	//	fixedstream.push(buf, 1);
	//	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	//}

	// 2. data_len_stream
	//rola::data_len_stream datalenstream;
	//datalenstream.matched_signal().connect([](std::vector<char> v) {
	//	std::cout << "received len: " << std::dec << v.size() << "\n";
	//	for (auto i : v)
	//	{
	//		std::cout << std::hex << (int)i << " ";
	//	}
	//	std::cout << "\n";
	//	});

	//char buf[4];
	//rola::host_to_network(8, buf);
	//char buf_data[8];

	//char buf2[4];
	//rola::host_to_network(32, buf2);
	//char buf2_data[32];

	//char buf3[4];
	//rola::host_to_network(20, buf3);
	//char buf3_data[20];

	////datalenstream.push(buf, 4);
	////datalenstream.push(buf_data, 8);
	////datalenstream.push(buf2, 4);
	////datalenstream.push(buf2_data, 32);
	////datalenstream.push(buf3, 4);
	////datalenstream.push(buf3_data, 20);

	////for (int i = 0; i < 4; ++i)
	////	datalenstream.push(buf + i, 1);
	////for (int i = 0; i < 8; ++i)
	////	datalenstream.push(buf_data + i, 1);
	////for (int i = 0; i < 4; ++i)
	////	datalenstream.push(buf2 + i, 1);
	////for (int i = 0; i < 32; ++i)
	////	datalenstream.push(buf2_data + i, 1);
	////for (int i = 0; i < 4; ++i)
	////	datalenstream.push(buf3 + i, 1);
	////for (int i = 0; i < 20; ++i)
	////	datalenstream.push(buf3_data + i, 1);

	//std::vector<char> v;
	//auto bi = std::back_inserter(v);
	//std::copy(buf, buf + 4, bi);
	//std::copy(buf_data, buf_data + 8, bi);
	//std::copy(buf2, buf2 + 4, bi);
	//std::copy(buf2_data, buf2_data + 32, bi);
	//std::copy(buf3, buf3 + 4, bi);
	//std::copy(buf3_data, buf3_data + 20, bi);
	//datalenstream.push(v.data(), v.size());

	// 3. head_tail stream
	rola::head_tail_stream<> head_tail({ 1,2,3,4 }, { 4,3,2,1 });
	head_tail.matched_signal().connect([](std::vector<char> v) {
	std::cout << "received len: " << std::dec << v.size() << "\n";
	for (auto i : v)
	{
		std::cout << std::hex << (int)i << " ";
	}
	std::cout << "\n";
	});
	char buf[512];
	buf[1] = 1;
	buf[2] = 2;
	buf[3] = 3;
	buf[4] = 4;

	buf[15] = 4;
	buf[16] = 3;
	buf[17] = 2;
	buf[18] = 1;

	buf[19] = 1;
	buf[20] = 2;
	buf[21] = 3;
	buf[22] = 4;

	buf[45] = 4;
	buf[46] = 3;
	buf[47] = 2;
	buf[48] = 1;
	head_tail.push(buf, 512);

	std::cout << "stream parser successful\n";
	return 0;
}