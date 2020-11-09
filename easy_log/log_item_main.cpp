#include "log_item.hpp"
#include <iostream>

using namespace std;
using namespace rola;

int main3()
{
	log_item log1(EasyLogLevel::EASY_LOG_FATAL, __FILE__, __func__, __LINE__);
	log1 << "some information";
	log1 << " " << 3456;
	std::cout << log1 << endl;

	log1 = std::move((LOG(TRACE) << "a log item: trace"));
	cout << log1 << endl;

	log1 = LOG(DEBUG) << "a log item: debug";
	cout << log1 << endl;

	log1 = LOG(INFO) << "a log item: info";
	cout << log1 << endl;

	log1 = std::move(LOG(WARN) << "a log item: warn");
	cout << log1 << endl;

	log1 = LOG(ERROR) << "a log item: error";
	cout << log1 << endl;

	log1 = LOG(FATAL) << "a log item: fatal";
	cout << log1 << endl;
	return 0;
}