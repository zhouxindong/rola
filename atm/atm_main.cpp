#include <thread>
#include "messages.hpp"
#include "atm_busi.hpp"
#include "bank_machine.hpp"
#include "interface_machine.hpp"

int main()
{
	rola::atm::bank_machine bank;
	rola::atm::interface_machine interface_hardware;

	rola::atm::atm machine(bank.get_sender(), interface_hardware.get_sender());

	std::thread bank_thread(&rola::atm::bank_machine::run, &bank);
	std::thread if_thread(&rola::atm::interface_machine::run, &interface_hardware);
	std::thread atm_thread(&rola::atm::atm::run, &machine);

	rola::csp::sender atmqueue(machine.get_sender());
	bool quit_pressed = false;
	while (!quit_pressed)
	{
		char c = getchar();
		switch (c)
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			atmqueue.send(rola::atm::digit_pressed(c));
			break;

		case 'b':
			atmqueue.send(rola::atm::balance_pressed());
			break;

		case 'w':
			atmqueue.send(rola::atm::withdraw_pressed(50));
			break;

		case 'c':
			atmqueue.send(rola::atm::cancel_pressed());
			break;

		case 'q':
			quit_pressed = true;
			break;

		case 'i':
			atmqueue.send(rola::atm::card_inserted("acc1234"));
			break;
		}
	}

	bank.done();
	machine.done();
	interface_hardware.done();
	atm_thread.join();
	bank_thread.join();
	if_thread.join();
	return 0;
}