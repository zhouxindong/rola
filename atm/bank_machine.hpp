#ifndef ROLA_ATM_BANK_MACHINE_HPP
#define ROLA_ATM_BANK_MACHINE_HPP

#include <csp/receiver.hpp>
#include <csp/sender.hpp>
#include "messages.hpp"

namespace rola 
{
	namespace atm
	{
		class bank_machine
		{
			csp::receiver incoming;
			unsigned balance;

		public:
			bank_machine()
				: balance(199) {}

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
							.handle<verify_pin>(
								[&](verify_pin const& msg)
								{
									if (msg.pin == "1937")
									{
										msg.atm_queue.send(pin_verified());
									}
									else
									{
										msg.atm_queue.send(pin_incorrect());
									}
								}
								)
							.handle<withdraw>(
								[&](withdraw const& msg)
								{
									if (this->balance >= msg.amount)
									{
										msg.atm_queue.send(withdraw_ok());
										this->balance -= msg.amount;
									}
									else
									{
										msg.atm_queue.send(withdraw_denied());
									}
								}
								)
							.handle<get_balance>(
								[&](get_balance const& msg)
								{
									msg.atm_queue.send(rola::atm::balance(this->balance));
								}
								)
							.handle<withdrawal_processed>(
								[&](withdrawal_processed const& msg) 
								{
								}
								)
							.handle<cancel_withdrawal>(
								[&](cancel_withdrawal const& msg) 
								{
								}
								);
					}
				}
				catch (csp::close_queue const&)
				{
				}
			}

			csp::sender get_sender()
			{
				return incoming;
			}
		};
	} // namespace atm
} // namespace rola

#endif // !ROLA_ATM_BANK_MACHINE_HPP
