#ifndef ROLA_EASY_BUS_MESSAGE_DISPATCHER_HPP
#define ROLA_EASY_BUS_MESSAGE_DISPATCHER_HPP

#include "message.hpp"

namespace rola
{
	// the dispatcher and handler(Func) for Msg
	// if dismatched, dispatch to PreviousDispatcher
	template <typename PreviousDispatcher, typename Msg, typename Func>
	class Template_dispatcher
	{
		Message_queue* mq_;
		PreviousDispatcher* prev_;
		Func f_;
		bool chained_;
		bool processed_{ false };

		Template_dispatcher(Template_dispatcher const&) = delete;
		Template_dispatcher& operator=(Template_dispatcher const&) = delete;

		template <typename Dispatcher, typename OtherMsg, typename OtherFunc>
		friend class Template_dispatcher; // all instances of Template_dispatcher is friendly each other

		void wait_and_dispatch()
		{
			for (;;)
			{
				auto msg = mq_->wait_and_pop();
				if (dispatch(msg)) // if handled msg, break to loop
					break;
			}
		}

		bool dispatch(SMB_shp const& msg)
		{
			auto p = cast_message<Msg>(msg);
			if (p.first) // matched message type
			{
				f_(p.second);
				return true;
			}
			else
			{
				return prev_->dispatch(msg); // dismatched, dispatch chain
			}
		}

	public:
		Template_dispatcher(Template_dispatcher&& other) noexcept
			: mq_(other.mq_)
			, prev_(other.prev_)
			, f_(std::move(other.f_))
			, chained_(other.chained_)
		{
			other.chained_ = true;
		}

		Template_dispatcher(Message_queue* mq, PreviousDispatcher* prev, Func&& f)
			: mq_(mq)
			, prev_(prev)
			, f_(std::forward<Func>(f))
			, chained_(false)
		{
			prev->chained_ = true;
		}

		template <typename OtherMsg, typename OtherFunc>
		Template_dispatcher<Template_dispatcher, OtherMsg, OtherFunc>
			handle(OtherFunc&& of) // can be link addition handler
		{
			return Template_dispatcher<Template_dispatcher, OtherMsg, OtherFunc>(
				mq_, this, std::forward<OtherFunc>(of));
		}

		void process()
		{
			if (!chained_)
				wait_and_dispatch();
			processed_ = true;
		}

		~Template_dispatcher() noexcept(false)
		{
			if (!chained_ && !processed_)
				wait_and_dispatch();
		}
	};

	// the message to close queue
	class Message_close_queue
	{
	public:
		static std::string class_name()
		{
			return "__EasyBus@Message_close_queue__";
		}

		std::string serialize() const
		{
			return std::string{};
		}

		static int32_t deserialize(std::string& str, Message_close_queue& mc)
		{
			return 0;
		}
	};

	// the start point to build dispatch chain
	// the end point to handle message
	// can't handle any type message but Message_close and to exit
	class Dispatcher
	{
		Message_queue* mq_;
		bool chained_;
		bool throwed_{ false };

		Dispatcher(Dispatcher const&) = delete;
		Dispatcher& operator=(Dispatcher const&) = delete;

		template <typename Dispatcher, typename Msg, typename Func>
		friend class Template_dispatcher; // require for dispatch chain

		void wait_and_dispatch()
		{
			for (;;) // loop, wait and dispatch message
			{
				auto msg = mq_->wait_and_pop();
				dispatch(msg);
			}
		}

		bool dispatch(SMB_shp const& msg)
		{
			auto p = cast_message<Message_close_queue>(msg);
			if (p.first)
			{
				throwed_ = true;
				throw Message_close_queue();
			}

			else // message no handler, end of dispatcher chain
				return false;
		}

	public:
		Dispatcher(Dispatcher&& other) noexcept
			: mq_(other.mq_)
			, chained_(other.chained_)
		{
			other.chained_ = true;
			book_messages<Message_close_queue>();
		}

		explicit Dispatcher(Message_queue* mq)
			: mq_(mq)
			, chained_(false)
		{
			book_messages<Message_close_queue>();
		}

		template <typename Message, typename Func>
		Template_dispatcher<Dispatcher, Message, Func>
			handle(Func&& f)
		{ // add a dispatcher can handle Message and add to chain above this object 
			return Template_dispatcher<Dispatcher, Message, Func>(
				mq_, this, std::forward<Func>(f));
		}

		void process()
		{
			if (!chained_)
				wait_and_dispatch();
		}

		~Dispatcher() noexcept(false)
		{
			if (!chained_ && !throwed_)
				wait_and_dispatch();
		}
	};
} // namespace rola

#endif // !ROLA_EASY_BUS_MESSAGE_DISPATCHER_HPP
