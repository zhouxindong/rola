#include "log_formatter.hpp"
#include "log_item.hpp"

#include <iostream>

using namespace std;
using namespace rola;

int main4()
{
	log_item log1(EasyLogLevel::EASY_LOG_FATAL, __FILE__, __func__, __LINE__);
	log1 << "some information";
	log1 << " " << 3456;
	cout << line_formatter<log_item>::format(log1) << endl;

	cout << table_formatter<log_item>::format(log1) << endl;

	log1 = std::move((LOG(TRACE) << "a log item: trace"));
	cout << line_formatter<log_item>::format(log1) << endl;
	cout << table_formatter<log_item>::format(log1) << endl;

	log1 = LOG(DEBUG) << "a log item: debug";
	cout << line_formatter<log_item>::format(log1) << endl;
	cout << table_formatter<log_item>::format(log1) << endl;

	log1 = LOG(INFO) << "a log item: info";
	cout << line_formatter<log_item>::format(log1) << endl;
	cout << table_formatter<log_item>::format(log1) << endl;

	log1 = std::move(LOG(WARN) << "a log item: warn");
	cout << line_formatter<log_item>::format(log1) << endl;
	cout << table_formatter<log_item>::format(log1) << endl;

	log1 = LOG(ERROR) << "a log item: error";
	cout << line_formatter<log_item>::format(log1) << endl;
	cout << table_formatter<log_item>::format(log1) << endl;

	log1 = LOG(FATAL) << "a log item: fatal";
	cout << line_formatter<log_item>::format(log1) << endl;
	cout << table_formatter<log_item>::format(log1) << endl;

	return 0;
}