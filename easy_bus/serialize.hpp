#ifndef ROLA_EASY_BUS_SERIALIZE_HPP
#define ROLA_EASY_BUS_SERIALIZE_HPP

#include <string>
#include <type_traits>

#include "utils/byte_order.hpp"

namespace rola
{
#pragma region serialize/deserialize

	/// <summary>
	/// class member function obj.serialize() or Class::serialize() detect
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name=""></typeparam>
	template <typename T, typename = std::string>
	struct Has_serialize : std::false_type
	{};

	template <typename T>
	struct Has_serialize<T, decltype(std::declval<T>().serialize())> : std::true_type
	{};

	template <typename T>
	std::enable_if_t<Has_serialize<T>::value, std::string>
		serialize(T const& obj)
	{
		return obj.serialize();
	}

	template <typename T>
	std::enable_if_t<Has_serialize<T>::value, std::string>
		serialize(T& obj)
	{
		return obj.serialize();
	}

	/// <summary>
	/// class member function Class::deserialize() detect
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name=""></typeparam>
	template <typename T, typename = T>
	struct Has_deserialize : std::false_type
	{};

	template <typename T>
	struct Has_deserialize<T, decltype(T::deserialize(std::declval<std::string>()))> : std::true_type
	{};

	template <typename T, typename = std::enable_if_t<Has_deserialize<T>::value>>
	void deserialize(std::string const& s, T& t)
	{
		t = T::deserialize(s);
	}

	// macro for no need for byte order transform
#define NORMAL_DATA_SERIALIZE(Type)						\
	std::string serialize(Type a)						\
	{													\
		std::string str;								\
		str.append((const char*)&a, sizeof(a));			\
		return str;										\
	}

#define NORMAL_DATA_DESERIALIZE(Type)					\
	void deserialize(std::string const& s, Type& v)		\
	{													\
		::memcpy(&v, s.data(), sizeof(v));				\
	}

	// macro for need for byte order transform
#define ORDER_DATA_SERIALIZE(Type)						\
	std::string serialize(Type a)						\
	{													\
		std::string s;									\
		Type n = rola::host_to_network(a);				\
		s.append((const char*)&n, sizeof(n));			\
		return s;										\
	}

#define ORDER_DATA_DESERIALIZE(Type)					\
	void deserialize(std::string const& s, Type& v)		\
	{													\
		::memcpy(&v, s.data(), sizeof(v));				\
		v = rola::network_to_host(v);					\
	}

	NORMAL_DATA_SERIALIZE(float)
	NORMAL_DATA_DESERIALIZE(float)
	NORMAL_DATA_SERIALIZE(double)
	NORMAL_DATA_DESERIALIZE(double)
	NORMAL_DATA_SERIALIZE(int8_t)
	NORMAL_DATA_DESERIALIZE(int8_t)
	NORMAL_DATA_SERIALIZE(uint8_t)
	NORMAL_DATA_DESERIALIZE(uint8_t)

	ORDER_DATA_SERIALIZE(int16_t)
	ORDER_DATA_DESERIALIZE(int16_t)
	ORDER_DATA_SERIALIZE(uint16_t)
	ORDER_DATA_DESERIALIZE(uint16_t)
	ORDER_DATA_SERIALIZE(int32_t)
	ORDER_DATA_DESERIALIZE(int32_t)
	ORDER_DATA_SERIALIZE(uint32_t)
	ORDER_DATA_DESERIALIZE(uint32_t)
	ORDER_DATA_SERIALIZE(int64_t)
	ORDER_DATA_DESERIALIZE(int64_t)
	ORDER_DATA_SERIALIZE(uint64_t)
	ORDER_DATA_DESERIALIZE(uint64_t)

		template<>
	string serialize(const string& a)
	{
		int len = a.size();
		string ans;
		ans.append(::serialize(len));
		ans.append(a);
		return ans;
	}
	template<>
	int deserialize(string str, string& a)
	{
		int len;
		::deserialize(str, len);
		a = str.substr(sizeof(len), len);
		return sizeof(int) + len;
	}

	template<typename SerializableType>
	class Serializable
	{
	public:
		static SerializableType deserialize(string);
		static string serialize(const SerializableType& a);
	};


	class OutEngine
	{
	public:
		template<typename SerializableType>
		OutEngine& operator << (SerializableType& a)
		{
			string x = ::serialize(a);
			os.write(x.data(), x.size());
			return *this;
		}
		string str()
		{
			return os.str();
		}
		void set_empty()
		{
			os.str("");
		}
		OutEngine() :os(std::ios::binary) {}
	public:
		ostringstream os;
	}

	class InEngine
	{
	public:
		InEngine(string s) : is(s) { n_size = leftsize(); }
		template<typename SerializableType>
		InEngine& operator >> (SerializableType& a)
		{
			int ret = ::deserialize(is, a);
			is = is.substr(ret);
			return *this;
		}
		void set_str(string s)
		{
			is = s;
			n_size = leftsize();
		}

		int leftsize()
		{
			return is.size();
		}
		int donesize()
		{
			return n_size - leftsize();
		}

	protected:
		string is;
		int n_size;
	}

#pragma endregion

} // namespace rola

#endif // !ROLA_EASY_BUS_SERIALIZE_HPP
