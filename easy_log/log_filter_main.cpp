#include "log_filter.hpp"
#include "log_channel.hpp"
#include "log_formatter.hpp"
#include "log_item.hpp"

#include <thread>
#include <chrono>

int main/*_lfm*/()
{
	auto v = rola::generate_log_item(100);

	rola::console_channel<rola::log_item, rola::line_formatter<rola::log_item>> terminal;

	rola::level_filter<decltype(terminal)> filter(
		terminal, rola::EasyLogLevel::EASY_LOG_WARN);
	for (auto& item : v)
	{
		filter(item);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	rola::file_channel<rola::log_item, rola::line_formatter<rola::log_item>, 50000> fc("set_filter_demo.out");
	auto v2 = rola::generate_log_item(10000);
	rola::set_filter<decltype(fc)> filter2(fc, 
		{rola::EasyLogLevel::EASY_LOG_TRACE, rola::EasyLogLevel::EASY_LOG_FATAL});
	for (auto& item : v2)
	{
		filter2(item);
	}
	return 0;
}