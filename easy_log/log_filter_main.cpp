#include "log_filter.hpp"
#include "log_channel.hpp"
#include "log_formatter.hpp"
#include "log_item.hpp"

#include <thread>
#include <chrono>

int main_lfm()
{
	auto v = rola::generate_log_item(100);

	rola::console_channel<rola::log_item, rola::line_formatter<rola::log_item>> terminal;

	rola::level_filter<rola::log_item, rola::line_formatter<rola::log_item>, rola::EasyLogLevel::EASY_LOG_WARN> filter;
	for (auto& item : v)
	{
		filter(terminal, item);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	rola::file_channel<rola::log_item, rola::line_formatter<rola::log_item>, 50000> fc("set_filter_demo.out");
	auto v2 = rola::generate_log_item(10000);
	rola::set_filter<
		rola::log_item, rola::line_formatter<rola::log_item>,
		rola::EasyLogLevel::EASY_LOG_TRACE, 
		rola::EasyLogLevel::EASY_LOG_FATAL> filter2;
	for (auto& item : v2)
	{
		filter2(fc, item);
	}

	return 0;
}