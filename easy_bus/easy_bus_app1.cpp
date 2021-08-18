#include <iostream>
#include "easy_bus/message_box.hpp"
#include "easy_bus/message_demo.hpp"
#include "easy_bus/app_base.hpp"


int main_2()
{
	message_table_init();

	rola::Mono_busapp app1("127.0.0.1", 8888);

	auto dispatch = app1.box().wait()
		.handle<MessageA>([&](MessageA const& msg) {
		std::cout << "app1::MessageA: " << msg.name << std::endl;
			}).handle<MessageB>([&](MessageB const& msg) {
				std::cout << "app1::MessageB: " << msg.age << std::endl;
				}).handle<MessageC>([&](MessageC const& msg) {
					std::cout << "app1::MessageC: " << msg.phones.size() << std::endl;
					});

				std::thread t(&rola::Mono_busapp::run<decltype(dispatch)>, &app1, std::move(dispatch));

				system("pause");
				app1.box().send(MessageA{ "abcdef" }, "127.0.0.1", 8888);

				system("pause");
				app1.box().send(MessageB{ 3829.2324 }, "127.0.0.1", 8888);

				system("pause");
				app1.box().send(MessageC{ { 1,2,3,4,5} }, "127.0.0.1", 8888);

				system("pause");
				app1.done();
				t.join();
				std::cout << "Hello World!\n";

				return 0;
}

int main_3()
{
	message_table_init();

	rola::Mono_busapp app1("127.0.0.1", 8888);

	auto dispatch = app1.box().wait()
		.handle<MessageA>([&](MessageA const& msg) {
		std::cout << "app1::MessageA: " << msg.name << std::endl;
			}).handle<MessageB>([&](MessageB const& msg) {
				std::cout << "app1::MessageB: " << msg.age << std::endl;
				}).handle<MessageC>([&](MessageC const& msg) {
					std::cout << "app1::MessageC: " << msg.phones.size() << std::endl;
					});

				std::thread t(&rola::Mono_busapp::run<decltype(dispatch)>, &app1, std::move(dispatch));

				system("pause");

				int count = 0;
				std::thread t2([&]() {
					while (count < 10000)
					{
						++count;
						switch (count % 3)
						{
						case 0:
							app1.box().send(MessageA{ std::to_string(count) }, "127.0.0.1", 9999);
							break;

						case 1:
							app1.box().send(MessageB{ (double)count }, "127.0.0.1", 9999);
							break;

						case 2:
							app1.box().send(MessageC{ {1,2,3,4,5,6} }, "127.0.0.1", 9999);
							break;

						default:
							break;
						}
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
					});

				system("pause");
				app1.done();

				t.join();
				t2.join();

				std::cout << "Hello World!\n";
				return 0;
}

class Foo : public rola::FSM_busapp<Foo>
{
public:
	Foo(std::string const& ip, in_port_t port)
		: FSM_busapp<Foo>(ip, port)
	{
		state = &Foo::process_in_stateA;
	}

	void process_in_stateA()
	{
		box().wait()
			.handle<MessageA>([&](MessageA const& msg) {
			std::cout << "app1::stateA::MessageA: " << msg.name << std::endl;
				}).handle<MessageB>([&](MessageB const& msg) {
					std::cout << "app1::stateA::MessageB: " << msg.age << std::endl;
					}).handle<MessageC>([&](MessageC const& msg) {
						std::cout << "app1::stateA::MessageC: " << msg.phones.size() << std::endl;
						state = &Foo::process_in_stateB;
						});
	}

	void process_in_stateB()
	{
		box().wait()
			.handle<MessageA>([&](MessageA const& msg) {
			std::cout << "app1::stateB::MessageA: " << msg.name << std::endl;
				}).handle<MessageB>([&](MessageB const& msg) {
					std::cout << "app1::stateB::MessageB: " << msg.age << std::endl;
					}).handle<MessageC>([&](MessageC const& msg) {
						std::cout << "app1::stateB::MessageC: " << msg.phones.size() << std::endl;
						state = &Foo::process_in_stateA;
						});
	}
};

int main()
{
	message_table_init();

	Foo foo("127.0.0.1", 8888);

	std::thread t(&Foo::run, &foo);

	system("pause");

	int count = 0;
	std::thread t2([&]() {
		while (count < 100000)
		{
			++count;
			switch (count % 3)
			{
			case 0:
				foo.box().send(MessageA{ std::to_string(count) }, "127.0.0.1", 9999);
				break;

			case 1:
				foo.box().send(MessageB{ (double)count }, "127.0.0.1", 9999);
				break;

			case 2:
				foo.box().send(MessageC{ {1,2,3,4,5,6} }, "127.0.0.1", 9999);
				break;

			default:
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(40));
		}
		});

	system("pause");
	foo.done();

	t.join();
	t2.join();

	std::cout << "Hello World!\n";
	return 0;
}