#include <iostream>
#include "inbox_hub.hpp"
#include "utils/stack_tracer.hpp"

struct MessageA
{
	std::string name;
	MessageA()
		: name{ "MessageA" }
	{
	}

	static int count;
};

int MessageA::count = 0;

struct MessageB
{
	std::string name;
	MessageB()
		: name{ "MessageB" }
	{
	}

	static int count;
};

int MessageB::count = 0;

struct MessageC
{
	std::string name;
	MessageC()
		: name{ "MessageC" }
	{
	}

	static int count;
};

int MessageC::count = 0;

int main()
{
	rola::inbox_hub& hub = rola::get_inboxes();

	rola::inbox box1;
	rola::inbox box2;


	box1.handle<MessageA>([&](const MessageA& m) {
		std::cout << "box1: " << m.name << ", MessageA count: " << ++MessageA::count << "\n";
		box2.send(MessageB{});
		//std::this_thread::sleep_for(std::chrono::milliseconds(10));
		})
		.handle<MessageB>([&](const MessageB& m) {
			std::cout << "box1: " << m.name << ", MessageB count: " << ++MessageB::count << "\n";
			box2.send(MessageC{});
			//std::this_thread::sleep_for(std::chrono::milliseconds(10));
			})
		.handle<MessageC>([&](const MessageC& m) {
				std::cout << "box1: " << m.name << ", MessageC count: " << ++MessageC::count << "\n";
				box2.send(MessageA{});
				//std::this_thread::sleep_for(std::chrono::milliseconds(10));
			});

	box2.handle<MessageA>([&](const MessageA& m) {
		std::cout << "box2: " << m.name << ", MessageA count: " << ++MessageA::count << "\n";
		box1.send(MessageB{});
		//std::this_thread::sleep_for(std::chrono::milliseconds(10));
		})
		.handle<MessageB>([&](const MessageB& m) {
			std::cout << "box2: " << m.name << ", MessageB count: " << ++MessageB::count << "\n";
			box1.send(MessageC{});
			//std::this_thread::sleep_for(std::chrono::milliseconds(10));
			})
		.handle<MessageC>([&](const MessageC& m) {
				std::cout << "box2: " << m.name << ", MessageC count: " << ++MessageC::count << "\n";
				box1.send(MessageA{});
				//std::this_thread::sleep_for(std::chrono::milliseconds(10));
			});

	hub.book_inbox("box1", &box1);
	hub.book_inbox("box2", &box2);

	box1.start();
	box2.start();

	box1.send(MessageA{});

	std::cout << "press any key to exit\n";
	std::cin.get();
	hub.stop();

	std::cout << "inbox_hub successful\n";
	return 0;
}