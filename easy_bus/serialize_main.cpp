#include "serialize.hpp"
#include <iostream>
#include <cassert>

class Foo
{
public:
	std::string name;
	int age;
	double height;

	Foo(std::string n, int a, double h)
		: name(n), age(a), height(h)
	{}

	Foo() = default;

public:
	static std::string class_name()
	{
		return "Foo";
	}

	std::string serialize() const
	{
		rola::object_serializer_helper ser;
		ser << name << age << height;
		return ser.str();
	}

	static int32_t deserialize(std::string& str, Foo& foo)
	{
		rola::object_deserializer_helper dser(str);

		dser >> foo.name >> foo.age >> foo.height;
		return 0;
	}
};

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

int main_serialize()
{
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

	bool b2;
	char c2;
	int8_t i8_2;
	uint8_t u8_2;
	int16_t i16_2;
	uint16_t u16_2;
	int32_t i32_2;
	uint32_t u32_2;
	int64_t i64_2;
	uint64_t u64_2;
	float f2;
	double d2;

	rola::deserialize(bs, b2);
	rola::deserialize(cs, c2);
	rola::deserialize(i8s, i8_2);
	rola::deserialize(u8s, u8_2);
	rola::deserialize(i16s, i16_2);
	rola::deserialize(u16s, u16_2);
	rola::deserialize(i32s, i32_2);
	rola::deserialize(u32s, u32_2);
	rola::deserialize(i64s, i64_2);
	rola::deserialize(u64s, u64_2);
	rola::deserialize(fs, f2);
	rola::deserialize(ds, d2);

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
	assert(f == f2);
	assert(d == d2);

	std::string s = "abcdefghijklmnopqrstuvwxyz";
	std::string ss = rola::serialize(s);
	std::string s2;
	rola::deserialize(ss, s2);
	assert(s == s2);

	int ary[10] = { 1,2,3,4,5,6,7,8,9,10 };
	std::string ary_s = rola::serialize(ary);
	int ary_2[10];
	int len = rola::deserialize(ary_s, ary_2);
	for (int i = 0; i < 10; ++i)
	{
		assert(ary[i] == ary_2[i]);
	}
	assert(len == 0);

	// std::vector
	std::vector<double> v{ 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9 };
	std::string v_s = rola::serialize(v);
	std::vector<double> v2;
	len = rola::deserialize(v_s, v2);
	for (int i = 0; i < 9; ++i)
	{
		assert(v[i] == v2[i]);
	}
	assert(len == 0);

	std::vector<std::string> vs{ "hello", "world", "hello world" };
	std::string vs_s = rola::serialize(vs);
	std::vector<std::string> vs_2;
	rola::deserialize(vs_s, vs_2);
	for (int i = 0; i < vs.size(); ++i)
	{
		assert(vs[i] == vs_2[i]);
	}

	std::vector<Foo> vec_foo;
	vec_foo.push_back(Foo("foo1", 32, 234.32));
	vec_foo.push_back(Foo("foo2", 42, 132.32));
	std::string vec_foo_s = rola::serialize(vec_foo);
	std::vector<Foo> vec_foo2;
	rola::deserialize(vec_foo_s, vec_foo2);
	for (auto& item : vec_foo2)
	{
		std::cout << item << std::endl;
	}
	assert(vec_foo == vec_foo2);

	// custom class
	Foo foo;
	foo.name = "Foo object";
	foo.age = 283;
	foo.height = 884.283;

	std::ostringstream oss;
	oss << rola::serialize(foo.class_name()) << rola::serialize(foo);
	std::string foo_ser = oss.str();

	std::string foo_name2;
	auto len3 = rola::deserialize(foo_ser, foo_name2);
	foo_ser = foo_ser.substr(len3);
	Foo foo2;
	rola::deserialize(foo_ser, foo2);
	assert(foo == foo2);
	assert(foo.class_name() == foo_name2);

	// pair
	std::pair<std::string, Foo> pf;
	pf.first = "pair.first";
	pf.second = foo;
	std::string pf_s = rola::serialize(pf);
	std::pair<std::string, Foo> pf2;
	rola::deserialize(pf_s, pf2);
	assert(pf.first == pf2.first);
	assert(pf.second.name == pf2.second.name);
	assert(pf.second.age == pf2.second.age);
	assert(pf.second.height == pf2.second.height);
	assert(pf == pf2);

	// map
	std::map<std::string, Foo> m;
	m["first"] = foo;

	Foo foo3;
	foo3.name = "foo object2";
	foo3.age = 32;
	foo3.height = 23.1;
	m["second"] = foo3;
	std::string m_s = rola::serialize(m);

	std::map<std::string, Foo> m2;
	rola::deserialize(m_s, m2);
	assert(m == m2);

	// empty string
	std::string s_empty;
	std::string ss_empty = rola::serialize(s_empty);
	std::string s_empty2;
	rola::deserialize(ss_empty, s_empty2);
	assert(s_empty == s_empty2);

	std::cout << "serialize_main successful" << std::endl;
	return 0;
}