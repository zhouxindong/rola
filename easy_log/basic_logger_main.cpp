#include "basic_logger.hpp"

#include <thread>
#include <chrono>

using namespace rola;

int main_blm()
{
	auto& logger = console_logger::get("console1");
	auto v = std::move(generate_log_item(2));
	for (auto& item : v)
	{
		logger.log(item);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	auto& logger2 = console_info_logger::get("console2");
	v = std::move(generate_log_item(200, " new log"));
	for (auto& item : v)
	{
		logger2.log(item);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	return 0;
}

int main_blm2()
{
	auto& logger = file_logger::get("testlog");
	auto v = std::move(generate_log_item(200));
	for (auto& item : v)
	{
		logger.log(item);
	}

	auto& logger2 = file_logger::get("testlog");
	v = std::move(generate_log_item(200, " new log"));
	for (auto& item : v)
	{
		logger2.log(item);
	}

	return 0;
}

int main_udplogger()
{
	auto& logger = udp_logger::get("172.10.10.155::8080");
	auto v = std::move(generate_log_item(200));
	for (auto& item : v)
	{
		logger.log(item);
	}

	auto& logger2 = udp_info_logger::get("172.10.10.155::8080");
	auto v2 = std::move(generate_log_item(200));
	for (auto& item : v2)
	{
		logger2.log(item);
	}

	std::cout << "basic_logger_main.cpp successful\n";

	return 0;
}