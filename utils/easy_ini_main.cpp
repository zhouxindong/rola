#include "easy_ini.hpp"
#include <iostream>
#include <cassert>
#include <string>

using namespace std;

int main_easyini()
{
	std::string ini_file = "rdv2_config.ini";
	Easy_ini inier(ini_file);
	assert(0 == inier.ParseError());


	string section1 = "VDR";
	assert(1811 ==
		(int)inier.GetInteger(section1, "VDR_id", 0));
	assert(string("224.0.0.155") ==	inier.Get(section1, "VDR_group_ip", ""));

	string section2 = "cmd";
	assert(inier.GetBoolean(section2, "cmd_enable", false));

	//assert(inier.Get(section2, "cmd_name", "") == u8"运行控制指令");

	std::cout << "easy_ini_main successful\n";
	return 0;
}
