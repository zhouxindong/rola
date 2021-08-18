#ifndef ROLA_EASY_PIPO_INBOX_HPP
#define ROLA_EASY_PIPO_INBOX_HPP

#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>
#include <unordered_map>
#include <functional>
#include <typeinfo>
#include <exception>
#include <sstream>
#include <utility>
#include <thread>
#include <future>
#include <string>
#include <atomic>

namespace rola
{
	namespace pipo_detail
	{
		/// <summary>
		/// the base class of all message class
		/// </summary>
		struct message_base
		{
			virtual std::type_info const* msg_typeinfo() const = 0;
                        virtual ~message_base() noexcept = 0;
		};

		inline message_base::~message_base() noexcept
		{}

		/// <summary>
		/// generic class contains a object of special class message
		/// </summary>
		/// <typeparam name="Msg"></typeparam>
		template <typename Msg>
		struct wrapped_message : message_base
		{
			Msg contents;

			explicit wrapped_message(Msg const& contents_)
				: contents(contents_)
			{}

			explicit wrapped_message(Msg&& contents_)
				: contents(std::move(contents_))
			{}

			std::type_info const* msg_typeinfo() const override
			{
				return &typeid(Msg);
			}
		};

		/// <summary>
		/// a queue contains messages to be handle
		/// </summary>
		class queue
		{
			std::mutex m;
			std::condition_variable c;
			std::queue<std::shared_ptr<message_base>> q;

		public:
			template <typename T>
			void push(T const& msg)
			{
				std::lock_guard<std::mutex> lk(m);
				q.push(std::make_shared<wrapped_message<T>>(msg));
				c.notify_all();
			}

			std::shared_ptr<message_base> wait_and_pop()
			{
				std::unique_lock<std::mutex> lk(m);
				c.wait(lk, [&] {return !q.empty(); });
				auto res = q.front();
				q.pop();
				return res;
			}
		};

		/// <summary>
		/// exception class for message handler operations
		/// </summary>
		class message_handler_exception : public std::exception
		{
			std::string msg_;

		public:
			explicit message_handler_exception(const std::string& msg)
				: msg_(msg)
			{}

			const char* what() const noexcept override
			{
				return msg_.c_str();
			}
		};

		/// <summary>
		/// a special message to notify is the time to close queue
		/// </summary>
		class close_queue_msg
		{};

		/// <summary>
		/// a helper function that to downcast message_base to wrapped message object
		/// </summary>
		/// <typeparam name="Msg"></typeparam>
		/// <param name="msg"></param>
		/// <returns></returns>
		template <typename Msg>
		static Msg downcast_msg(std::shared_ptr<rola::pipo_detail::message_base> msg)
		{
			if (rola::pipo_detail::wrapped_message<Msg>* wrapper =
				dynamic_cast<rola::pipo_detail::wrapped_message<Msg>*>(msg.get()))
			{
				return wrapper->contents;
			}

			std::ostringstream oss;

			oss << "bad called downcast_msg from [" << msg->msg_typeinfo()->name() << "] to ["
				<< typeid(Msg).name() << "]";
			throw std::runtime_error(oss.str().c_str());
		}

		/// <summary>
		/// transform F call argument signature from Msg to message base
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <typeparam name="F"></typeparam>
		/// <param name="f"></param>
		/// <returns></returns>
		template <typename T, typename F>
		static auto wrapped_handle(F&& f)
		{
			return [f](std::shared_ptr<rola::pipo_detail::message_base> msg)
			{
				auto content = downcast_msg<T>(msg);
				f(content);
			};
		}

		/// <summary>
		/// message handler class
		/// book, query
		/// </summary>
		class message_handler
		{
			std::unordered_map <
				std::type_info const*,
				std::function<void(std::shared_ptr<message_base>)>> msg_handler_map_;

			std::mutex m_;

		public:
			template <typename T, typename F>
			void book(F&& f)
			{
				auto ti = &typeid(T);
				std::lock_guard<std::mutex> lg(m_);

				if (msg_handler_map_.find(ti) != msg_handler_map_.end())
				{
					std::ostringstream oss;
					oss << "the type [" << ti->name() << "] handler is already exists";
					throw message_handler_exception(oss.str().c_str());
				}

				msg_handler_map_[ti] = wrapped_handle<T>(std::forward<F>(f));
			}

			std::pair<bool, std::function<void(std::shared_ptr<message_base>)>>
				query(std::shared_ptr<message_base> msg)
			{
				auto ti = msg->msg_typeinfo();
				std::lock_guard<std::mutex> lg(m_);

				auto iter = msg_handler_map_.find(ti);
				if (iter == msg_handler_map_.end())
					return std::make_pair(false, [](std::shared_ptr<message_base>) {});
				return std::make_pair(true, iter->second);
			}
		};
	} // namespace pipo_detail

	/// <summary>
	/// a thread execution with a message queue
	/// </summary>
	class inbox
	{
	public:
		inbox()
		{
			handle<pipo_detail::close_queue_msg>([](const pipo_detail::close_queue_msg&) {
				throw pipo_detail::close_queue_msg{};
				});
		}

	public:
		template <typename T, typename F>
		inbox& handle(F&& f)
		{
			handlers_.book<T>(std::forward<F>(f));
			return *this;
		}

		template <typename Msg>
		void send(Msg const& msg)
		{
			q_.push(msg);
		}

		void start()
		{
			future_ = std::async(std::launch::async,
				[this]() {
					for (;;)
					{
						auto msg = q_.wait_and_pop();
						auto hdl = handlers_.query(msg);
						if (!hdl.first)
							continue;

						try
						{
							hdl.second(msg);
						}
						catch (pipo_detail::close_queue_msg&)
						{
							break;
						}
					}
				});
		}

		void stop()
		{
			if (stopped_)
				return;

			stopped_ = true;
			q_.push(pipo_detail::close_queue_msg{});
		}

	private:
		pipo_detail::queue q_;
		pipo_detail::message_handler handlers_;
		std::future<void> future_;

	private:
		std::atomic_bool stopped_{ false };
	};
} // namespace rola

#endif // ROLA_EASY_PIPO_INBOX_HPP
