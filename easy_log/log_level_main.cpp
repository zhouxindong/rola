#include "log_level.hpp"
#include <cassert>
#include <iostream>
#include <string>

using namespace std;

int main2()
{
	assert(string("TRACE")	== string(rola::log_level_text(rola::EasyLogLevel::EASY_LOG_TRACE)));
	assert(string("DEBUG")	== string(rola::log_level_text(rola::EasyLogLevel::EASY_LOG_DEBUG)));
	assert(string("INFO")	== string(rola::log_level_text(rola::EasyLogLevel::EASY_LOG_INFO)));
	assert(string("WARN")	== string(rola::log_level_text(rola::EasyLogLevel::EASY_LOG_WARN)));
	assert(string("ERROR")	== string(rola::log_level_text(rola::EasyLogLevel::EASY_LOG_ERROR)));
	assert(string("FATAL")	== string(rola::log_level_text(rola::EasyLogLevel::EASY_LOG_FATAL)));
	std::cout << "log_level test successful\n";
	return 0;
}