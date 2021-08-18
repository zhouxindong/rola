#include "message.hpp"
#include "dispatcher.hpp"
#include "sender.hpp"
#include <iostream>
#include <string>
#include "receiver.hpp"

struct MessageA
{
	static constexpr int value = 1;
};

struct MessageB
{
	static constexpr int value = 2;
};

struct MessageC
{
	static constexpr int value = 3;
};

class bank_machine
{
	rola::csp2::receiver incoming;

public:
	bank_machine()
	{}

	void done()
	{
		get_sender().send(rola::csp2::close_queue());
	}

	void run()
	{
		auto dispatch = incoming.wait()
			.handle<MessageA>(
				[&](MessageA const& msg)
				{
					std::cout << msg.value << std::endl;
				}
				)
			.handle<MessageB>(
				[&](MessageB const& msg)
				{
					std::cout << msg.value << std::endl;
				}
				)
					.handle<MessageC>(
						[&](MessageC const& msg)
						{
							std::cout << msg.value << std::endl;
						}

				);

				try
				{
					for (;;)
					{
						dispatch.process();
					}
				}
				catch (rola::csp2::close_queue const&)
				{
				}
	}

	rola::csp2::sender get_sender()
	{
		return incoming;
	}
};


int main()
{
	bank_machine machine;
	std::thread t(&bank_machine::run, &machine);

	system("pause");
	machine.get_sender().send(MessageB());
	system("pause");
	machine.get_sender().send(MessageA());
	system("pause");
	machine.get_sender().send(MessageC());
	system("pause");
	machine.done();
	t.join();
	std::cout << "csp2_test successful" << std::endl;
	return 0;
}