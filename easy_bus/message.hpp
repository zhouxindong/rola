#ifndef ROLA_EASY_BUS_MESSAGE_HPP
#define ROLA_EASY_BUS_MESSAGE_HPP

#include <utility>
#include <unordered_map>
#include <functional>
#include <optional>
#include <memory>

#include "serialize.hpp"

namespace rola
{
	// std::string .class_name() detect
	template <typename T, typename = std::string>
	struct Has_class_name : std::false_type
	{};

	template <typename T>
	struct Has_class_name<T, decltype(std::declval<T const>().class_name())> : std::true_type
	{};

	// base class: Serializable_message_base
	class Serializable_message_base
	{
	public:
		virtual ~Serializable_message_base() = 0
		{}

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
	class Serializable_message : public Serializable_message_base
	{
#pragma region constrains and storage

		static_assert(Has_class_name<T>::value, "T must support class_name() member function");
		static_assert(Has_serialize<T>::value, "T must support serialize() member function");
		static_assert(Has_deserialize<T>::value, "T must support deserialize() class function");

		std::optional<T> object_;

#pragma endregion

#pragma region ctor

	public:
		Serializable_message() = default;
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
			object_.reset(rhs.object_);
		}

		Serializable_message& operator=(Serializable_message&& rhs)
		{
			object_.reset(std::move(rhs.object_));
		}

		virtual ~Serializable_message() {}

#pragma endregion

#pragma region interfaces

		std::optional<T>& object()
		{
			return object_;
		}

		std::optional<T> const& object() const
		{
			return object_;
		}

#pragma endregion

#pragma region overrides

	public:
		std::string class_name() const override
		{
			if (!object_)
				return std::string();
			return (*object_).class_name();
		}

		std::string serialize() const override
		{
			if (!object_)
				return std::string();

			std::ostringstream oss;
			oss << rola::serialize(class_name()) << rola::serialize(*object_);
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

	// deserialize message
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
		Message m;
		rola::class_deserializer_map()[m.class_name()] = deserialize_message<Message>;
	}

	// query the deserializer for special class name
	inline std::optional<std::function<SMB_shp(std::string&)>>
		lookup_deserializer(std::string const& name)
	{
		auto& map = class_deserializer_map();
		auto it = map.find(name);
		return (it == map.end()) ? std::nullopt : std::make_optional(it->second);
	}

#pragma endregion
} // namespace rola

#endif // !ROLA_EASY_BUS_MESSAGE_HPP
