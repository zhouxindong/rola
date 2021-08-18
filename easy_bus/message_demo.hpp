#include <string>
#include <vector>
#include "message.hpp"
#include <algorithm>

class MessageA
{
public:
	std::string name;

	MessageA() = default;
	explicit MessageA(std::string n)
		: name(n)
	{}

	static std::string class_name()
	{
		return "MessageA";
	}

	std::string serialize() const
	{
		rola::object_serializer_helper ser;
		ser << name;
		return ser.str();
	}

	static int32_t deserialize(std::string& str, MessageA& ma)
	{
		rola::object_deserializer_helper dser(str);
		dser >> ma.name;
		return 0;
	}
};

class MessageB
{
public:
	double age;

	MessageB() = default;
	explicit MessageB(double a)
		: age(a)
	{}

	static std::string class_name()
	{
		return "MessageB";
	}

	std::string serialize() const
	{
		rola::object_serializer_helper ser;
		ser << age;
		return ser.str();
	}

	static int32_t deserialize(std::string& str, MessageB& mb)
	{
		rola::object_deserializer_helper dser(str);
		dser >> mb.age;
		return 0;
	}
};

class MessageC
{
public:
	std::vector<int> phones;

	MessageC() = default;
	explicit MessageC(std::vector<int> const& v)
	{
		phones.resize(v.size());
		std::copy(std::begin(v), std::end(v), std::begin(phones));
	}

	static std::string class_name()
	{
		return "MessageC";
	}

	std::string serialize() const
	{
		rola::object_serializer_helper ser;
		ser << phones;
		return ser.str();
	}

	static int32_t deserialize(std::string& str, MessageC& mc)
	{
		rola::object_deserializer_helper dser(str);
		dser >> mc.phones;
		return 0;
	}
};

inline void message_table_init()
{
	rola::book_messages<MessageA, MessageB, MessageC>();
}