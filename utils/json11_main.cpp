#include "json11.hpp"

#include <iostream>

using namespace std;
using namespace rola;

int main_json11()
{
	const string simple_test =
		R"({"k1":"v1", "k2":42, "k3":["a",123,true,false,null], "sub":{"kv1":"vv1"}})";

	string err;
	const auto json = Json::parse(simple_test, err);

	std::cout << "k1: " << json["k1"].string_value() << "\n";
	std::cout << "k2: " << json["k2"].int_value() << endl;
	std::cout << "k3: " << json["k3"].dump() << "\n";
	
	auto k1 = json["k1"].string_value();
	auto k2 = json["k2"].int_value();
	auto k3 = json["k3"].array_items();
	auto sub = json["sub"].object_items();

	auto kv1 = sub["kv1"].string_value();

	for (auto& k : json["k3"].array_items()) {
		std::cout << "    - " << k.dump() << "\n";
	}

	auto root = rola::Easy_json::from_file("test_utf8.json");
	assert(root);
	auto nonexist = rola::Easy_json::from_file("nonexist.json");
	assert(!nonexist);

	auto phones = root->value("phone").to_array();
	assert(phones.size() == 2);
	assert(phones[0].to_string() == "12345");
	assert(phones[1].to_string() == "49283");

	auto married = root->value("married").to_bool();
	assert(married);

	auto height = root->value("height").to_double();
	assert(height == 179.86);
	auto age = root->value("age").to_int();
	assert(age == 36);

	auto hobby = root->value("hobby").to_object();
	auto hobby_abc = hobby["abc"].to_int();
	assert(hobby_abc == 33);
	auto hobby_ds = hobby["ds"].to_string();
	assert(hobby_ds == "json");

	auto name = root->value("name").to_string();
	//std::string zs = u8"张三";
	//assert(name == zs);

	//rola::JsonObject o;
	//o.insert("name", u8"张三");
	//std::cout << rola::Easy_json::to_json(o) << "\n";
	//rola::Easy_json::to_file("test_out1.json", o);

	//{"k1":"v1", "k2" : 42, "k3" : ["a", 123, true, false, null] , "sub" : {"kv1":"vv1"}}
	rola::JsonObject o;
	o.insert("k1", "v1");
	o.insert("k2", 42);
	
	rola::JsonArray ary;
	ary << "a" << 123 << true << false;
	o.insert("k3", ary);

	rola::JsonObject sub2;
	sub2.insert("kv1", "vv1");
	o.insert("sub", sub2);
	//rola::Easy_json::to_file("test_out1.json", o);

	std::cout << "\n----------------------------\n";
	auto o2 = rola::Easy_json::from_file("test_out1.json");
	std::cout << o2->dump() << "\n";
	std::cout << "json11_main successful\n";
	return 0;
}