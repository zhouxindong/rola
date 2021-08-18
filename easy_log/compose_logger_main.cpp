#include "compose_logger.hpp"
#include "log_item.hpp"
#include "log_formatter.hpp"

#include <thread>
#include <chrono>

//#include "stlex/localtime_utils.hpp"

using namespace std;
using namespace rola;

int main()
{
	using level_filter = level_filter<log_item, line_formatter<log_item>, rola::EasyLogLevel::EASY_LOG_TRACE>;
	using base_logger_item = logger_item_base<log_item, line_formatter<log_item>>;
	using csl = console_logger_item<log_item, line_formatter<log_item>, level_filter>;
	using sefilter = set_filter<log_item, line_formatter<log_item>, rola::EasyLogLevel::EASY_LOG_DEBUG, rola::EasyLogLevel::EASY_LOG_ERROR>;
	using fil = file_logger_item<log_item, line_formatter<log_item>, sefilter>;
	using uli = udp_logger_item<log_item, line_formatter<log_item>, level_filter>;

	//std::shared_ptr<base_logger_item> pconsole
	//	= std::make_shared<csl>();

	//auto v = rola::generate_log_item(100);

	//for (auto& item : v)
	//{
	//	pconsole->log(item);
	//	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	//}

	//std::shared_ptr<base_logger_item> pfile = std::make_shared<fil>("comitem323", "llog");
	//v = std::move(rola::generate_log_item(100));
	//for (auto& item : v)
	//{
	//	pfile->log(item);
	//}

	compose_logger<log_item, line_formatter<log_item>> com_logger;
	com_logger.add</*level_filter,*/ csl>();
	com_logger.add</*sefilter,*/ fil>("comitem323", "llog");
	com_logger.add<uli>("172.10.10.155:8080");

	auto v = rola::generate_log_item(100);

	for (auto& item : v)
	{
		com_logger.log(item);
		com_logger << item;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	std::cout << "compose_logger_main.cpp successful\n";

	return 0;
}