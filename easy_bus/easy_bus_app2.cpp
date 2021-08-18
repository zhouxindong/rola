// easy_bus_app2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "easy_bus/message_box.hpp"
#include "easy_bus/message_demo.hpp"
#include "easy_bus/app_base.hpp"
#include <chrono>

void handle_messageA(MessageA const& msg)
{
	std::cout << "app2::MessageA: " << msg.name << std::endl;
}

void handle_messageB(MessageB const& msg)
{
	std::cout << "app2::MessageB: " << msg.age << std::endl;
}

void handle_messageC(MessageC const& msg)
{
	std::cout << "app2::MessageC: " << msg.phones.size() << std::endl;
}

int main_app2()
{
	message_table_init();

	rola::Mono_busapp app1("127.0.0.1", 9999);

	//auto dispatch = app1.box().wait()
	//	.handle<MessageA>([&](MessageA const& msg) {
	//	std::cout << "app2::MessageA: " << msg.name << std::endl;
	//}).handle<MessageB>([&](MessageB const& msg) {
	//	std::cout << "app2::MessageB: " << msg.age << std::endl;
	//}).handle<MessageC>([&](MessageC const& msg) {
	//	std::cout << "app2::MessageC: " << msg.phones.size() << std::endl;
	//});

	auto dispatch = app1.box().wait()
		.handle<MessageA>(handle_messageA)
		.handle<MessageB>(handle_messageB)
		.handle<MessageC>(handle_messageC);

	std::thread t(&rola::Mono_busapp::run<decltype(dispatch)>, &app1, std::move(dispatch));

	system("pause");

	int count = 0;
	std::thread t2([&]() {
		while (count < 100000)
		{
			++count;
			switch (count % 3)
			{
			case 0:
				app1.box().send(MessageA{ std::to_string(count) }, "127.0.0.1", 8888);
				break;

			case 1:
				app1.box().send(MessageB{ (double)count }, "127.0.0.1", 8888);
				break;

			case 2:
				app1.box().send(MessageC{ {1,2,3,4,5,6} }, "127.0.0.1", 8888);
				break;

			default:
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(40));
		}
		});

	system("pause");
	app1.done();

	t.join();
	t2.join();

	std::cout << "Hello World!\n";
	return 0;
}