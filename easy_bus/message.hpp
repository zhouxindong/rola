#ifndef ROLA_EASY_BUS_MESSAGE_HPP
#define ROLA_EASY_BUS_MESSAGE_HPP

#include <utility>
#include <unordered_map>
#include <functional>
#include <memory>
#include <type_traits>
#include <mutex>
#include <condition_variable>
#include <queue>

#include "serialize.hpp"
#include "mpl/type_list.hpp"

namespace rola
{
	// std::string T::class_name() detect
	template <typename T, typename = std::string>
	struct Has_class_name : std::false_type
	{};

	template <typename T>
	struct Has_class_name<T, decltype(T::class_name())> : std::true_type
	{};

	// base class: Serializable_message_base
	class Serializable_message_base
	{
	public:
		virtual ~Serializable_message_base() noexcept
		{}

		Serializable_message_base() = default;

		// prevent object to slice
		Serializable_message_base(Serializable_message_base const&) = delete;
		Serializable_message_base& operator=(Serializable_message_base const&) = delete;

	public:
		// deserialize the header data for class name
		static std::string deserialize_class_name(std::string& s)
		{
			std::string name;
			auto len = rola::deserialize(s, name);
			s = s.substr(len);
			return name;
		}

	public:
		virtual std::string class_name() const = 0;
		virtual std::string serialize() const = 0;
		virtual std::shared_ptr<Serializable_message_base> deserialize(std::string& s) const = 0;
	};

	// serializable message body class
	template <typename T>
	class Serializable_message final : public Serializable_message_base
	{
#pragma region constrains and storage

		static_assert(Has_class_name<T>::value, "T must support class_name() class function");
		static_assert(Has_serialize<T>::value, "T must support serialize() member function");
		static_assert(Has_deserialize<T>::value, "T must support deserialize() class function");

		T object_;

#pragma endregion

#pragma region ctor

	public:
		Serializable_message() : object_{}
		{}

		template <typename U, std::enable_if_t<std::is_convertible_v<U, T>>>
		Serializable_message(U const& o)
			: object_(o)
		{}

		template <typename U, std::enable_if_t<std::is_convertible_v<U, T>>>
		Serializable_message(Serializable_message<U> const& o)
			: object_(o.object_)
		{}

		Serializable_message(T const& o)
			: object_(o)
		{}

		Serializable_message(Serializable_message const& rhs)
			: object_(rhs.object_)
		{}

		Serializable_message(Serializable_message&& rhs)
			: object_(std::move(rhs.object_))
		{}

		Serializable_message& operator=(Serializable_message const& rhs)
		{
			object_ = rhs.object_;
		}

		Serializable_message& operator=(Serializable_message&& rhs)
		{
			object_ = std::move(rhs.object_);
		}

		virtual ~Serializable_message() noexcept {}

		void swap(Serializable_message& rhs) noexcept
		{
			std::swap(object_, rhs.object_);
		}

#pragma endregion

#pragma region interfaces

		T& object()
		{
			return object_;
		}

		T const& object() const
		{
			return object_;
		}

#pragma endregion

#pragma region overrides

	public:
		std::string class_name() const override
		{
			return T::class_name();
		}

		std::string serialize() const override
		{
			std::ostringstream oss;
			oss << rola::serialize(class_name()) << rola::serialize(object_);
			return oss.str();
		}

		std::shared_ptr<Serializable_message_base> deserialize(std::string& s) const override
		{
			T t;
			rola::deserialize(s, t);
			return std::make_shared<Serializable_message>(t);
		}

#pragma endregion
	}; // class Serializable_message<T>

	template <typename T>
	inline void swap(Serializable_message<T>& a, Serializable_message<T>& b)
	{
		a.swap(b);
	}

#pragma region object serialize/deserialize framework

	// result type for deserialize
	using SMB_shp = std::shared_ptr<Serializable_message_base>;

	// serialize message
	template <typename Message>
	inline std::string serialize_message(Message const& msg)
	{
		auto shd = std::make_shared<Serializable_message<Message>>(msg);
		return shd->serialize();
	}

	// deserialize message, used for lookup-table entry
	template <typename Message>
	inline SMB_shp deserialize_message(std::string& s)
	{
		auto shp = std::make_shared<Serializable_message<Message>>();
		return shp->deserialize(s);
	}

	// global map object: lookup table for class deserialize
	inline
		std::unordered_map<std::string, std::function<SMB_shp(std::string&)>>&
		class_deserializer_map()
	{
		static std::unordered_map<std::string, std::function<SMB_shp(std::string&)>> m;
		return m;
	}

	// book the class object deserialize entry
	template <typename Message>
	inline void book_deserializable_message()
	{
		rola::class_deserializer_map()[Message::class_name()] = deserialize_message<Message>;
	}

	// query the deserializer for special class name
	inline std::pair<bool, std::function<SMB_shp(std::string&)>>
		lookup_deserializer(std::string const& name)
	{
		auto& map = class_deserializer_map();
		auto it = map.find(name);
		return (it == map.end()) ?
			std::make_pair(false, std::function<SMB_shp(std::string&)>{}) :
			std::make_pair(true, it->second);
	}

#pragma endregion

#pragma region book message helper

	// book message

	template <typename T>
	struct Book_message
	{
		Book_message()
		{
			rola::book_deserializable_message<T>();
		}
	};

	template <typename ... Ts>
	inline void book_messages()
	{
		rola::TL::Apply<rola::Type_list<Ts...>, Book_message> bm;
	}

	// rebuild message

	inline std::shared_ptr<Serializable_message_base> rebuild_message(std::string& s)
	{
		try
		{
			auto class_name = Serializable_message_base::deserialize_class_name(s);

			auto deser = lookup_deserializer(class_name);
			if (!deser.first)
				return SMB_shp();

			return (deser.second)(s);
		}
		catch (...)
		{
			return SMB_shp();
		}
	}

	template <typename Message>
	inline std::pair<bool, Message> cast_message(SMB_shp const& smb)
	{
		if (!smb)
			return std::make_pair(false, Message{});

		Serializable_message<Message>* body =
			dynamic_cast<Serializable_message<Message>*>(smb.get());
		if (body == nullptr)
			return std::make_pair(false, Message{});
		else
			return std::make_pair(true, body->object());
	}

	template <typename Message>
	inline std::pair<bool, Message> rebuild_special_message(std::string& s)
	{
		auto obj_p = rebuild_message(s);
		if (!obj_p)
			return std::make_pair(false, Message{});
		return cast_message<Message>(obj_p);
	}

#pragma endregion

	// message queue
	class Message_queue
	{
		std::mutex m_;
		std::condition_variable cv_;
		std::queue<SMB_shp> q_;

	public:
		void push(SMB_shp sh)
		{
			std::lock_guard<std::mutex> lg(m_);
			q_.push(sh);
			cv_.notify_all();
		}

		template <typename Message>
		void push(Message const& msg)
		{
			push(std::make_shared<Serializable_message<Message>>(msg));
		}

		SMB_shp wait_and_pop()
		{
			std::unique_lock<std::mutex> ul(m_);
			cv_.wait(ul, [&] {return !q_.empty(); });
			auto m = q_.front();
			q_.pop();
			return m;
		}
	};

	//class 
} // namespace rola
#endif // !ROLA_EASY_BUS_MESSAGE_HPP

