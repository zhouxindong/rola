#ifndef ROLA_EASY_BUS_APP_BASE_HPP
#define ROLA_EASY_BUS_APP_BASE_HPP

#include "message_box.hpp"

namespace rola
{
	// simplify the app used easy bus(exchange message throught message box)
	class Busapp_base
	{
	protected:
		Busapp_base(std::string const& ip, in_port_t port)
			: box_(ip, port)
			, ip_(ip)
			, port_(port)
		{}

		Busapp_base() = delete;
		Busapp_base(Busapp_base const&) = delete;
		Busapp_base(Busapp_base&&) = delete;
		Busapp_base& operator=(Busapp_base const&) = delete;
		Busapp_base& operator=(Busapp_base&&) = delete;

	public:
		// exit the run loop for dispatch messages
		void done()
		{
			box_.send(rola::Message_close_queue(), ip_, port_);
		}

		// message box: 
		// the start point to build messages dispatchers
		rola::Message_box& box()
		{
			return box_;
		}

	private:
		rola::Message_box box_;

	private:
		std::string ip_;
		in_port_t port_;
	};

	// mono state handle app
	class Mono_busapp : public Busapp_base
	{
	public:
		Mono_busapp(std::string const& ip, in_port_t port)
			: Busapp_base(ip, port)
		{}

		template <typename TDispatcher>
		void run(TDispatcher dispatch)
		{
			box().start();

			try
			{
				for (;;)
				{
					dispatch.process();
				}
			}
			catch (rola::Message_close_queue const&)
			{
			}

			box().stop();
		}
	};

	// finite state machine handle app
	template <typename Derived>
	class FSM_busapp : public Busapp_base
	{
	public:
		FSM_busapp(std::string const& ip, in_port_t port)
			: Busapp_base(ip, port)
		{}

	public:
		void(Derived::* state)() { nullptr };

		void run()
		{
			if (state == nullptr)
				return;

			box().start();

			try
			{
				for (;;)
				{
					(static_cast<Derived*>(this)->*state)();
				}
			}
			catch (rola::Message_close_queue const&)
			{
			}

			box().stop();
		}
	};
} // namespace rola

#endif // !ROLA_EASY_BUS_APP_BASE_HPP
