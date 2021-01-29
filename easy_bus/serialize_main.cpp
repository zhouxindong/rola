#include "serialize.hpp"
#include <iostream>
#include <cassert>

class Foo
{
public:
	std::string name;
	int age;
	double height;

public:
	std::string serialize()
	{
		rola::Data_serializer ser;
		ser << name << age << height;
		return ser.str();
	}

	static int32_t deserialize(std::string& str, Foo& foo)
	{
		rola::Data_deserializer dser(str);
		dser >> foo.name >> foo.age >> foo.height;
		return sizeof(foo);
	}
};

int main()
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
	assert(len == 44);

	std::vector<double> v{ 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9 };
	std::string v_s = rola::serialize(v);
	std::vector<double> v2;
	len = rola::deserialize(v_s, v2);
	for (int i = 0; i < 9; ++i)
	{
		assert(v[i] == v2[i]);
	}
	assert(len == 76);

	std::vector<std::string> vs{ "hello", "world", "hello world" };
	std::string vs_s = rola::serialize(vs);
	std::vector<std::string> vs_2;
	rola::deserialize(vs_s, vs_2);
	for (int i = 0; i < vs.size(); ++i)
	{
		assert(vs[i] == vs_2[i]);
	}

	Foo foo;
	foo.name = "Foo object";
	foo.age = 283;
	foo.height = 884.283;

	std::string foo_s = rola::serialize(foo);
	Foo foo2;
	rola::deserialize(foo_s, foo2);
	std::cout << "foo2::name = " << foo2.name << std::endl;
	std::cout << "foo2::age = " << foo2.age << std::endl;
	std::cout << "foo2::height = " << foo2.height << std::endl;

	std::cout << "serialize_main successful" << std::endl;
	return 0;
}

