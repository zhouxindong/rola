#include "log_item.hpp"
#include "log_formatter.hpp"
#include "log_channel.hpp"

#include <thread>
#include <chrono>

// roll_file
int main_lcm1()
{
	rola::roll_file<> rf("log2/log4/3log", "yspdb");

	return 0;
}

// null_channel
int main_lcm2()
{
	rola::null_channel<rola::log_item, rola::line_formatter<rola::log_item>> dummy;

	rola::log_item log1 = LOG(FATAL);
	log1 << "some information";
	log1 << " " << 3456;
	dummy << log1;

	log1 = std::move((LOG(TRACE) << "a log item: trace"));
	dummy << log1;

	log1 = LOG(DEBUG) << "a log item: debug";
	dummy << log1;

	log1 = LOG(INFO) << "a log item: info";
	dummy << log1;

	log1 = std::move(LOG(WARN) << "a log item: warn");
	dummy << log1;

	log1 = LOG(ERROR) << "a log item: error";
	dummy << log1;

	log1 = LOG(FATAL) << "a log item: fatal";
	dummy << log1;
	return 0;
}

// console channel
int main_lcm3()
{
	rola::console_channel<rola::log_item, rola::line_formatter<rola::log_item>> terminal;
	rola::log_item log1 = LOG(FATAL);
	log1 << "some information";
	log1 << " " << 3456;
	terminal << log1;

	log1 = std::move((LOG(TRACE) << "a log item: trace"));
	terminal << log1;

	log1 = LOG(DEBUG) << "a log item: debug";
	terminal << log1;

	log1 = LOG(INFO) << "a log item: info";
	terminal << log1;

	log1 = std::move(LOG(WARN) << "a log item: warn");
	terminal << log1;

	log1 = LOG(ERROR) << "a log item: error";
	terminal << log1;

	log1 = LOG(FATAL) << "a log item: fatal";
	terminal << log1;
	return 0;
}

// file channel
int main_lcm4()
{
	rola::file_channel<rola::log_item, rola::line_formatter<rola::log_item>, 1000000> 
		fc("file_channel1.out");

	for (int i = 0; i < 10000; ++i)
	{
		if (i % 100 == 0)
			std::cout << i << std::endl;

		rola::log_item log1 = LOG(FATAL);
		log1 << "some information";
		log1 << " " << 3456;
		fc << log1;

		log1 = std::move((LOG(TRACE) << "a log item: trace"));
		fc << log1;

		log1 = LOG(DEBUG) << "a log item: debug";
		fc << log1;

		log1 = LOG(INFO) << "a log item: info";
		fc << log1;

		log1 = std::move(LOG(WARN) << "a log item: warn");
		fc << log1;

		log1 = LOG(ERROR) << "a log item: error";
		fc << log1;

		log1 = LOG(FATAL) << "a log item: fatal";
		fc << log1;
	}
	return 0;
}

// udp channel
int main_udp_channel()
{
	rola::udp_channel<rola::log_item, rola::line_formatter<rola::log_item>>
		udpch("172.10.10.155:8080");


	auto v = std::move(rola::generate_log_item(100));
	for (auto& item : v)
	{
		udpch.log(item);

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	std::cout << "log_channel_main.cpp successful\n";
	return 0;
}