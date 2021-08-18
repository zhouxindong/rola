#ifndef ROLA_CSP_DISPATCHER_HPP
#define ROLA_CSP_DISPATCHER_HPP

#include "message.hpp"

namespace rola
{
	namespace csp2
	{
		template <typename PreviousDispatcher, typename Msg, typename Func>
		class TemplateDispatcher
		{
			queue* q;
			PreviousDispatcher* prev;
			Func f;
			bool chained;

			TemplateDispatcher(TemplateDispatcher const&) = delete;
			TemplateDispatcher& operator=(TemplateDispatcher const&) = delete;

			template <typename Dispatcher, typename OtherMsg, typename OtherFunc>
			friend class TemplateDispatcher; // all instances of TemplateDispatcher is friendly each other

			void wait_and_dispatch()
			{
				for (;;)
				{
					auto msg = q->wait_and_pop();
					if (dispatch(msg)) // if handled msg, break to loop
						break;
				}
			}

			bool dispatch(std::shared_ptr<message_base> const& msg)
			{
				if (wrapped_message<Msg>* wrapper =
					dynamic_cast<wrapped_message<Msg>*>(msg.get())) // check msg type and call handler is type matched
				{
					f(wrapper->contents);
					return true;
				}
				else
				{
					return prev->dispatch(msg); // link to previous dispatcher
				}
			}

		public:
			TemplateDispatcher(TemplateDispatcher&& other)
				: q(other.q)
				, prev(other.prev)
				, f(std::move(other.f))
				, chained(other.chained)
			{
				other.chained = true;
			}

			TemplateDispatcher(queue* q_, PreviousDispatcher* prev_, Func&& f_)
				: q(q_)
				, prev(prev_)
				, f(std::forward<Func>(f_))
				, chained(false)
			{
				prev_->chained = true;
			}

			template <typename OtherMsg, typename OtherFunc>
			TemplateDispatcher<TemplateDispatcher, OtherMsg, OtherFunc>
				handle(OtherFunc&& of) // can be link addition handler
			{
				return TemplateDispatcher<TemplateDispatcher, OtherMsg, OtherFunc>(
					q, this, std::forward<OtherFunc>(of));
			}

			void process()
			{
				if (!chained)
					wait_and_dispatch();
			}
		};

		class close_queue // the message to close queue
		{};

		class dispatcher
		{
			queue* q;
			bool chained;

			dispatcher(dispatcher const&) = delete;
			dispatcher& operator=(dispatcher const&) = delete;

			template<
				typename Dispatcher,
				typename Msg,
				typename Func>
				friend class TemplateDispatcher;

			void wait_and_dispatch()
			{
				for (;;) // loop, wait and dispatch message
				{
					auto msg = q->wait_and_pop();
					dispatch(msg);
				}
			}

			bool dispatch(std::shared_ptr<message_base> const& msg)
			{
				if (dynamic_cast<wrapped_message<close_queue>*>(msg.get())) // if is close_queue, throw exception
				{
					throw close_queue();
				}
				return false;
			}

		public:
			dispatcher(dispatcher&& other) noexcept
				: q(other.q)
				, chained(other.chained)
			{
				other.chained = true;
			}

			explicit dispatcher(queue* q_)
				: q(q_)
				, chained(false)
			{}

			template <typename Message, typename Func>
			TemplateDispatcher<dispatcher, Message, Func>
				handle(Func&& f)
			{
				return TemplateDispatcher<dispatcher, Message, Func>(
					q, this, std::forward<Func>(f));
			}

			void process()
			{
				if (!chained)
					wait_and_dispatch();
			}
		};
	} // namespace csp2
} // namespace rola
#endif // !ROLA_CSP_DISPATCHER_HPP
