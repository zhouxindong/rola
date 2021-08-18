#include "signal_hub.hpp"
#include <base/singlable.hpp>

#include <iostream>
#include <thread>
#include <chrono>

using namespace rola;
using namespace rola::base;

bool thread_slot(int s)
{
	std::cout << "thread_slot: " << s << "\n";
	return true;
}

bool main_slot(int s)
{
	std::cout << "main_slot: " << s << "\n";
	return true;
}

bool running = true;
std::string main_signal = "main_signal";
std::string thread_signal = "thread_signal";

void thread_proc(Signal_hub<bool(int)>* hub)
{
	auto* t_signal = hub->get_signal(thread_signal);

	auto* m_signal = hub->get_signal(main_signal);
	m_signal->connect(thread_slot);
	int i = 0;
	while (running)
	{
		t_signal->emit_(++i);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

class Foo
{
public:
	bool bar(std::string const& s)
	{
		std::cout << "Foo::bar(std::string)"  << s << std::endl;
		return true;
	}
};

int main_hub()
{
	Signal_hub<bool(int)>* sig_hub = Singlable<Signal_hub<bool(int)>>::instance_p();
	std::cout << "press any key to register main_signal\n";
	std::cin.get();
	auto* m_signal = sig_hub->get_signal(main_signal);

	std::cout << "press any key to start a thread with register thread_signal\n";
	std::cin.get();
	std::thread t(thread_proc, sig_hub);
	std::this_thread::sleep_for(std::chrono::seconds(1));

	sig_hub->get_signal(thread_signal)->connect(main_slot);

	for (int i = 0; i < 100; ++i)
	{
		m_signal->emit_(i);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	running = false;
	
	t.join();
	std::cout << "signal_hub test successful\n";
	return 0;
}