#include <iostream>
#include "serialize.hpp"
#include <cassert>
#include <array>

class Foo
{
public:
	std::string name{};
	int age{ 0 };
	double height{ 0 };

	Foo(std::string n, int a, double h)
		: name(n), age(a), height(h)
	{}

	Foo() = default;

public:
	std::string serialize() const
	{
		rola::object_serializer_helper ser;
		ser << name << age << height;
		return ser.str();
	}

	static Foo deserialize(std::string& str)
	{
		rola::object_deserializer_helper dser(str);
		Foo f;

		dser >> f.name >> f.age >> f.height;
		return f;
	}
};

bool operator<(Foo const& lhs, Foo const& rhs)
{
	return lhs.age < rhs.age;
}

bool operator==(Foo const& lhs, Foo const& rhs)
{
	return
		lhs.name == rhs.name &&
		lhs.age == rhs.age &&
		lhs.height == rhs.height;
}

std::ostream& operator<<(std::ostream& out, Foo const& f)
{
	out << "name: " << f.name << ", age: " << f.age << ", height: " << f.height;
	return out;
}

namespace std
{
	template <>
	struct hash<Foo>
	{
		std::size_t operator()(const Foo& value) const
		{
			return hash<std::string>{}(value.name) ^ hash<int>{}(value.age) ^ hash<double>{}(value.height);
		}
	};
}

int main_serialize()
{
	{ // in_built type serialize/deserialize
		bool b = true;
		char c = 'c';
		int8_t i8 = -99;
		uint8_t u8 = 214;
		int16_t i16 = -3245;
		uint16_t u16 = 20832;
		int32_t i32 = -1946455;
		uint32_t u32 = 25646346;
		int64_t i64 = -393426431743;
		uint64_t u64 = 29583825832235;
		float f = 325.36f;
		double d = 362983.235;

		std::string bs = rola::serialize(b);
		std::string cs = rola::serialize(c);
		std::string i8s = rola::serialize(i8);
		std::string u8s = rola::serialize(u8);
		std::string i16s = rola::serialize(i16);
		std::string u16s = rola::serialize(u16);
		std::string i32s = rola::serialize(i32);
		std::string u32s = rola::serialize(u32);
		std::string i64s = rola::serialize(i64);
		std::string u64s = rola::serialize(u64);
		std::string fs = rola::serialize(f);
		std::string ds = rola::serialize(d);

		bool b2 = rola::deserialize<bool>(bs);
		char c2 = rola::deserialize<char>(cs);
		int8_t i8_2 = rola::deserialize<int8_t>(i8s);
		uint8_t u8_2 = rola::deserialize<uint8_t>(u8s);
		int16_t i16_2 = rola::deserialize<int16_t>(i16s);
		uint16_t u16_2 = rola::deserialize<uint16_t>(u16s);
		int32_t i32_2 = rola::deserialize<int32_t>(i32s);
		uint32_t u32_2 = rola::deserialize<uint32_t>(u32s);
		int64_t i64_2 = rola::deserialize<int64_t>(i64s);
		uint64_t u64_2 = rola::deserialize<uint64_t>(u64s);
		float f2 = rola::deserialize<float>(fs);
		double d2 = rola::deserialize<double>(ds);

		assert(f == f2);
		assert(d == d2);
		assert(b == b2);
		assert(c == c2);
		assert(i8 == i8_2);
		assert(u8 == u8_2);
		assert(i16 == i16_2);
		assert(u16 == u16_2);
		assert(i32 == i32_2);
		assert(u32 == u32_2);
		assert(i64 == i64_2);
		assert(u64 == u64_2);
	}	

	{ // std::string
		std::string s = "abcdefghijklmnopqrstuvwxyz";
		std::string ss = rola::serialize(s);
		std::string s2 = rola::deserialize<std::string>(ss);
		assert(s == s2);

		// array[]
		int inary[10] = { 1,2,3,4,5,6,7,8,9,10 };
		std::string inary_s = rola::serialize(inary);
		int inary_2[10];
		rola::deserialize(inary_s, inary_2);
		for (int i = 0; i < 10; ++i)
		{
			assert(inary[i] == inary_2[i]);
		}

		std::string arys[3] = { "one", "two", "three" };
		std::string arys_ser = rola::serialize(arys);
		std::string arys_2[3];
		rola::deserialize(arys_ser, arys_2);
		for (int i = 0; i < 3; ++i)
		{
			assert(arys[i] == arys_2[i]);
		}

		// std::array<T, N>
		std::array<int, 10> ary{ 1,2,3,4,5,6,7,8,9,10 };
		std::string ary_s = rola::serialize(ary);
		auto ary_2 = rola::deserialize<std::array<int, 10>>(ary_s);
		for (int i = 0; i < 10; ++i)
		{
			assert(ary[i] == ary_2[i]);
		}

		// std::vector<T>
		std::vector<double> v{ 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9 };
		std::string v_s = rola::serialize(v);
		std::vector<double> v2 = rola::deserialize<std::vector<double>>(v_s);
		for (int i = 0; i < 9; ++i)
		{
			assert(v[i] == v2[i]);
		}

		std::vector<std::string> vs{ "hello", "world", "hello world" };
		std::string vs_s = rola::serialize(vs);
		std::vector<std::string> vs_2 = rola::deserialize<std::vector<std::string>>(vs_s);
		for (int i = 0; i < vs.size(); ++i)
		{
			assert(vs[i] == vs_2[i]);
		}
	}
	
	{ // custom class
		Foo foo("abcd", 123, 3.1415);
		std::string foo_s = rola::serialize(foo);
		Foo foo2 = rola::deserialize<Foo>(foo_s);
		assert(foo == foo2);

		std::vector<Foo> vec_foo;
		vec_foo.push_back(Foo("foo1", 32, 234.32));
		vec_foo.push_back(Foo("foo2", 42, 132.32));
		std::string vec_foo_s = rola::serialize(vec_foo);
		std::vector<Foo> vec_foo2 = rola::deserialize<std::vector<Foo>>(vec_foo_s);
		assert(vec_foo == vec_foo2);
	}

	{ // std::pair
		std::pair<std::string, Foo> pf;
		pf.first = "pair.first";
		pf.second = Foo("abcd", 123, 3.1415);
		std::string pf_s = rola::serialize(pf);
		std::pair<std::string, Foo> pf2 = rola::deserialize<decltype(pf)>(pf_s);
		assert(pf.first == pf2.first);
		assert(pf.second.name == pf2.second.name);
		assert(pf.second.age == pf2.second.age);
		assert(pf.second.height == pf2.second.height);
		assert(pf == pf2);
	}

	{ // set
		std::set<int> si{ 1,3,9,2 };
		std::string si_s = rola::serialize(si);
		std::set<int> si2 = rola::deserialize<std::set<int>>(si_s);
		assert(si2 == si);

		std::set<Foo> sf{ {"1", 1, 1}, {"2", 2, 2} };
		std::string sf_s = rola::serialize(sf);
		std::set<Foo> sf2 = rola::deserialize<std::set<Foo>>(sf_s);
		assert(sf == sf2);
	}

	{ // unordered_set
		std::unordered_set<int> si{ 1,3,9,2 };
		std::string si_s = rola::serialize(si);
		std::unordered_set<int> si2 = rola::deserialize<std::unordered_set<int>>(si_s);
		assert(si2 == si);

		std::unordered_set<Foo> sf{ {"1", 1, 1}, {"2", 2, 2} };
		std::string sf_s = rola::serialize(sf);
		std::unordered_set<Foo> sf2 = rola::deserialize<std::unordered_set<Foo>>(sf_s);
		assert(sf == sf2);
	}

	{ // map
		std::map<std::string, Foo> m{ {"one", {"1", 1, 1}}, {"two",{"2", 2, 2}} };
		std::string m_s = rola::serialize(m);
		std::map<std::string, Foo> m2 =
			rola::deserialize<std::map<std::string, Foo>>(m_s);
		assert(m == m2);
	}

	{ // unordered_map
		std::unordered_map<std::string, Foo> m{ {"one", {"1", 1, 1}}, {"two",{"2", 2, 2}} };
		std::string m_s = rola::serialize(m);
		std::unordered_map<std::string, Foo> m2 =
			rola::deserialize<std::unordered_map<std::string, Foo>>(m_s);
		assert(m == m2);
	}

	{ // tuple
		std::tuple<int, double, std::string> tup{ 100, 3.2, "hello" };
		std::string tup_s = rola::serialize(tup);
		
		auto tup2 = rola::deserialize<decltype(tup)>(tup_s);
		assert(tup == tup2);
	}

	std::cout << "serialize successful\n";
	return 0;
}