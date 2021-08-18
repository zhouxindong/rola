#ifndef ROLA_ATM_INTERFACE_MACHINE_HPP
#define ROLA_ATM_INTERFACE_MACHINE_HPP

#include <iostream>

#include <csp/receiver.hpp>
#include <csp/sender.hpp>
#include "messages.hpp"

namespace rola
{
	namespace atm
	{
		class interface_machine
		{
			csp::receiver incoming;
			std::mutex iom;

		public:
			csp::sender get_sender()
			{
				return incoming;
			}

			void done()
			{
				get_sender().send(csp::close_queue());
			}

			void run()
			{
				try
				{
					for (;;)
					{
						incoming.wait()
							.handle<issue_money>(
								[&](issue_money const& msg)
								{
									{
										std::lock_guard<std::mutex> lk(iom);
										std::cout << "Issuing " << msg.amount << std::endl;
									}
								}
								)
							.handle<display_insufficient_funds>(
								[&](display_insufficient_funds const& msg)
								{
									{
										std::lock_guard<std::mutex> lk(iom);
										std::cout << "Insufficient funds" << std::endl;
									}
								}
								)
							.handle<display_enter_pin>(
								[&](display_enter_pin const& msg)
								{
									{
										std::lock_guard<std::mutex> lk(iom);
										std::cout << "please enter you PIN (0-9)" << std::endl;
									}
								}
								)
							.handle<display_enter_card>(
								[&](display_enter_card const& msg) 
								{
									{
										std::lock_guard<std::mutex> lk(iom);
										std::cout << "please enter you card(I)" << std::endl;
									}
								}
								)
							.handle<display_balance>(
								[&](display_balance const& msg)
								{
									{
										std::lock_guard<std::mutex> lk(iom);
										std::cout << "the balance of your account is " << msg.amount << std::endl;
									}
								}
								)
							.handle<display_withdrawal_options>(
								[&](display_withdrawal_options const& msg) 
								{
									{
										std::lock_guard<std::mutex> lk(iom);
										std::cout << "withdraw 50? (w)" << std::endl;
										std::cout << "display balance? (b)" << std::endl;
										std::cout << "cancel? (c)" << std::endl;
									}
								}
								)
							.handle<display_withdrawal_cancelled>(
								[&](display_withdrawal_cancelled const& msg)
								{
									{
										std::lock_guard<std::mutex> lk(iom);
										std::cout << "withdrawal cancelled" << std::endl;
									}
								}
								)
							.handle<display_pin_incorrect_message>(
								[&](display_pin_incorrect_message const& msg)
								{
									{
										std::lock_guard<std::mutex> lk(iom);
										std::cout << "PIN incorrect" << std::endl;
									}
								}
								)
							.handle<eject_card>(
								[&](eject_card const& msg)
								{
									{
										std::lock_guard<std::mutex> lk(iom);
										std::cout << "Ejecting card" << std::endl;
									}
								});
							
					}
				}
				catch (csp::close_queue&)
				{
				}
			}
		};
	} // namespace atm
} // namespace rola

#endif // !ROLA_ATM_INTERFACE_MACHINE_HPP
