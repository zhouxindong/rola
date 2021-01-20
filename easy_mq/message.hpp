#ifndef ROLA_EASY_MQ_MESSAGE_HPP
#define ROLA_EASY_MQ_MESSAGE_HPP

#include <stdint.h>
#include <memory>
#include <atomic>

namespace rola
{
	namespace mq
	{
		using uid_t = uint64_t;

		inline uid_t gen_uid()
		{
			static std::atomic<uid_t> i(0);
			return ++i;
		}

		class Message
		{
		public:
			Message(int id)
				: id_(id)
				, unique_id_(gen_uid())
			{}

			virtual ~Message() = default;
			Message(Message const&) = delete;
			Message& operator=(Message const&) = delete;

		public:
			virtual std::unique_ptr<Message> move()
			{
				return std::unique_ptr<Message>(
					new Message(std::move(*this))
					);
			}

			int get_id() const
			{
				return id_;
			}

			uid_t get_uid() const
			{
				return unique_id_;
			}

		protected:
			Message(Message&&) = default;
			Message& operator=(Message&&) = default;

		private:
			int id_;
			uid_t unique_id_;
		};

		template <typename PayLoadType>
		class Message_data : public Message
		{
		public:
			template <typename ...Args>
			Message_data(int id, Args&&... args)
				: Message(id)
				, pl_(new PayLoadType(std::forward<Args>(args)...))
			{}

			virtual ~Message_data() = default;

			Message_data(Message_data const&) = delete;
			Message_data& operator=(Message_data const&) = delete;

		public:
			std::unique_ptr<Message> move() override
			{
				return std::unique_ptr<Message>(
					new Message_data<PayLoadType>(std::move(*this))
					);
			}

			PayLoadType& get_payload() const
			{
				return *pl_;
			}

		protected:
			Message_data(Message_data&&) = default;
			Message_data& operator=(Message_data&&) = default;
		private:
			std::unique_ptr<PayLoadType> pl_;
		};
	} // namespace mq
} // namespace rola

#endif // !ROLA_EASY_MQ_MESSAGE_HPP