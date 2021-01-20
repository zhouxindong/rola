#include "ntp_client.hpp"
#include <iostream>

void received_ntp_time(rola::Ntp_reply const& reply)
{
	std::cout << reply;
	std::cout << "--------------------------------------------" << std::endl;
}

int main()
{
	auto tp = std::chrono::system_clock::now();
	auto stamp = rola::Ntp_timestamp::from_timepoint(tp);
	auto tp2 = rola::Ntp_timestamp::to_timepoint(stamp);

	std::cout << "tp: " << tp.time_since_epoch().count() << std::endl;
	std::cout << "tp2: " << tp2.time_since_epoch().count() << std::endl;

	rola::Ntp_std_client ntp_client;
	rola::Ntp_std_client::get_signal(rola::Ntp_std_client::ntp_reply_signal())->connect(received_ntp_time);

	while (true)
	{
		ntp_client.send_request("129.6.15.28", 123);
		std::this_thread::sleep_for(std::chrono::seconds(3));
	}
	return 0;
}