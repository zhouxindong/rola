#include "message.hpp"
#include <iostream>
#include <cassert>

class Bar
{
public:
	std::string name;
	int age;
	double height;

	Bar(std::string n, int a, double h)
		: name(n), age(a), height(h)
	{}

	Bar() = default;

public:
	static std::string class_name()
	{
		return "Bar";
	}

	std::string serialize() const
	{
		rola::object_serializer_helper ser;
		ser << name << age << height;
		return ser.str();
	}

	static int32_t deserialize(std::string& str, Bar& foo)
	{
		rola::object_deserializer_helper dser(str);

		dser >> foo.name >> foo.age >> foo.height;
		return 0;
	}
};

bool operator==(Bar const& lhs, Bar const& rhs)
{
	return
		lhs.name == rhs.name &&
		lhs.age == rhs.age &&
		lhs.height == rhs.height;
}

std::ostream& operator<<(std::ostream& out, Bar const& f)
{
	out << "name: " << f.name << ", age: " << f.age << ", height: " << f.height;
	return out;
}

int main_message()
{
	// sender
	Bar b("bar1", 10, 3.2323);
	auto str = rola::serialize_message(b);

	// receiver
	rola::book_deserializable_message<Bar>();
	auto cname = rola::Serializable_message_base::deserialize_class_name(str);
	auto deser = rola::lookup_deserializer(cname);
	if (!deser.first)
	{
		std::cout << "can't retrieve the Bar deserializer\n";
	}
	else
	{
		auto de_bar = (deser.second)(str);
		rola::Serializable_message<Bar>* body =
			dynamic_cast<rola::Serializable_message<Bar>*>(de_bar.get());
		assert(body != nullptr);
		assert(body->object() == b);
	}

	std::cout << "message.cpp test successful\n";
	return 0;
}
