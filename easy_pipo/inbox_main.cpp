#include <iostream>
#include "inbox.hpp"
#include <string>
#include <cassert>

struct MessageA
{
	std::string name;
	MessageA()
		: name{ "MessageA" }
	{}
};

struct MessageB
{
	std::string name;
	MessageB()
		: name{ "MessageB" }
	{}
};

struct MessageC
{
	std::string name;
	MessageC()
		: name{ "MessageC" }
	{}
};

int main_inbox()
{
	rola::inbox incoming;

	incoming
		.handle<MessageA>([](const MessageA&) {
		std::cout << "handle message MessageA\n";
			})
		.handle<MessageC>([](const MessageC&) {
				std::cout << "handle message MessageB\n";
			});

	incoming.start();

	std::cout << "after incoming start\n";

	std::cout << "press any key to send MessageA\n";
	std::cin.get();
	incoming.send(MessageA{});

	std::cout << "press any key to send MessageB\n";
	std::cin.get();
	incoming.send(MessageB{});

	std::cout << "press any key to send MessageC\n";
	std::cin.get();
	incoming.send(MessageC{});

	std::cout << "press any key to stop\n";
	std::cin.get();
	incoming.stop();

	std::cout << "inbox main successful\n";
	return 0;
}