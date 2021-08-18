#ifndef ROLA_EASY_BUS_SERIALIZE_HPP
#define ROLA_EASY_BUS_SERIALIZE_HPP

#include <string>
#include <type_traits>
#include <sstream>
#include <vector>
#include <utility>
#include <map>

#include "utils/byte_order.hpp"

namespace rola
{
#pragma region serialize/deserialize

	/// <summary>
	/// class member function std::string obj.serialize() or std::string Class::serialize() detect
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name=""></typeparam>
	template <typename T, typename = std::string>
	struct Has_serialize : std::false_type
	{};

	template <typename T>
	struct Has_serialize<T, decltype(std::declval<T const>().serialize())> : std::true_type
	{};

	template <typename T>
	inline std::enable_if_t<Has_serialize<T>::value, std::string>
		serialize(T const& obj)
	{
		return obj.serialize(); // serialize() const
	}

	/// <summary>
	/// class member function T Class::deserialize(std::string) detect
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name=""></typeparam>
	template <typename T, typename = int32_t>
	struct Has_deserialize : std::false_type
	{};

	template <typename T>
	struct Has_deserialize<T, decltype(T::deserialize(std::declval<std::string&>(), std::declval<T&>()))> : std::true_type
	{};

	/// <summary>
	/// deserialize T object from string
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name=""></typeparam>
	/// <param name="s"></param>
	/// <param name="t"></param>
	/// <returns>means the offset after deserialize between the pointer of str and next data start point</returns>
	template <typename T, typename = std::enable_if_t<Has_deserialize<T>::value>>
	inline int32_t deserialize(std::string& s, T& t)
	{
		return T::deserialize(s, t);
	}

	// macro for no need for byte order transform
#define NORMAL_DATA_SERIALIZE(Type)						\
	inline std::string serialize(Type a)				\
	{													\
		std::string str;								\
		str.append((const char*)&a, sizeof(a));			\
		return str;										\
	}

#define NORMAL_DATA_DESERIALIZE(Type)					\
	inline												\
	int32_t deserialize(std::string& s, Type& v)		\
	{													\
		::memcpy(&v, s.data(), sizeof(v));				\
		return sizeof(v);								\
	}

	// macro for need for byte order transform
#define ORDER_DATA_SERIALIZE(Type)						\
	inline std::string serialize(Type a)				\
	{													\
		std::string s;									\
		Type n = rola::host_to_network(a);				\
		s.append((const char*)&n, sizeof(n));			\
		return s;										\
	}

#define ORDER_DATA_DESERIALIZE(Type)					\
	inline												\
	int32_t deserialize(std::string& s, Type& v)		\
	{													\
		::memcpy(&v, s.data(), sizeof(v));				\
		v = rola::network_to_host(v);					\
		return sizeof(v);								\
	}

	// overload for int32_t
	inline std::string serialize(int32_t a)
	{
		int32_t tmp = rola::host_to_network(a);
		std::string s;
		s.append((const char*)&tmp, sizeof(tmp));
		return s;
	}

	inline int32_t deserialize(std::string& s, int32_t& v)
	{
		::memcpy(&v, s.data(), sizeof(v));
		v = rola::network_to_host(v);
		return sizeof(v);
	}

	// overload for built-in numeric type
	NORMAL_DATA_SERIALIZE(float)
		NORMAL_DATA_DESERIALIZE(float)
		NORMAL_DATA_SERIALIZE(double)
		NORMAL_DATA_DESERIALIZE(double)
		NORMAL_DATA_SERIALIZE(int8_t)
		NORMAL_DATA_DESERIALIZE(int8_t)
		NORMAL_DATA_SERIALIZE(uint8_t)
		NORMAL_DATA_DESERIALIZE(uint8_t)
		NORMAL_DATA_SERIALIZE(bool)
		NORMAL_DATA_DESERIALIZE(bool)
		NORMAL_DATA_SERIALIZE(char)
		NORMAL_DATA_DESERIALIZE(char)

		ORDER_DATA_SERIALIZE(int16_t)
		ORDER_DATA_DESERIALIZE(int16_t)
		ORDER_DATA_SERIALIZE(uint16_t)
		ORDER_DATA_DESERIALIZE(uint16_t)
		ORDER_DATA_SERIALIZE(uint32_t)
		ORDER_DATA_DESERIALIZE(uint32_t)
		ORDER_DATA_SERIALIZE(int64_t)
		ORDER_DATA_DESERIALIZE(int64_t)
		ORDER_DATA_SERIALIZE(uint64_t)
		ORDER_DATA_DESERIALIZE(uint64_t)

		// overload for std::string
		inline std::string serialize(std::string const& a)
	{
		int32_t len = static_cast<int32_t>(a.size());
		std::string ans;
		ans.append(serialize(len));
		ans.append(a);
		return ans;
	}

	inline int32_t deserialize(std::string& str, std::string& a)
	{
		int32_t len;
		deserialize(str, len);
		a = str.substr(sizeof(len), len);
		return len + sizeof(int32_t);
	}

#pragma endregion

#pragma region compound and STL 

	// serialize struct just like array
	template <typename T>
	inline std::string linear_serialize(T const& v, uint32_t size)
	{
		std::string tmp;
		tmp.append(serialize(size));
		for (uint32_t i = 0; i < size; ++i)
		{
			tmp.append(serialize(v[i]));
		}
		return tmp;
	}

	inline int32_t linear_deserialize_size(std::string& str, uint32_t& size)
	{
		return deserialize(str, size);
	}

	template <typename T>
	inline int32_t liner_deserialize_data(std::string& str, T& v, uint32_t size)
	{
		int len;
		for (uint32_t i = 0; i < size; ++i)
		{
			len = deserialize(str, v[i]);
			str = str.substr(len);
		}
		return 0;
	}

	// build-in array
	template <typename T, uint32_t N>
	inline std::string serialize(T(&ary)[N])
	{
		return linear_serialize(ary, N);
	}

	template <typename T, uint32_t N>
	inline int32_t deserialize(std::string& str, T(&ary)[N])
	{
		uint32_t size;
		int ret = linear_deserialize_size(str, size);
		if (size != N)
			throw std::runtime_error("array size mismatched");

		str = str.substr(ret);
		return liner_deserialize_data(str, ary, size);
	}

	// vector
	template <typename T>
	inline std::string serialize(std::vector<T> const& v)
	{
		uint32_t size = static_cast<uint32_t>(v.size());
		return linear_serialize(v, size);
	}

	template <typename T>
	inline int32_t deserialize(std::string& str, std::vector<T>& v)
	{
		uint32_t size;
		int ret = linear_deserialize_size(str, size);
		str = str.substr(ret);
		v.resize(size);
		return liner_deserialize_data(str, v, size);
	}

	// pair
	template <typename Key, typename Value>
	inline std::string serialize(std::pair<Key, Value> const& v)
	{
		std::string tmp;
		tmp.append(serialize(v.first));
		tmp.append(serialize(v.second));
		return tmp;
	}

	template <typename Key, typename Value>
	inline int32_t deserialize(std::string& str, std::pair<Key, Value>& v)
	{
		int len_key = deserialize(str, v.first);
		str = str.substr(len_key);
		int len_value = deserialize(str, v.second);
		str = str.substr(len_value);
		return 0;
	}

	// map
	template <typename Key, typename Value>
	inline std::string serialize(std::map<Key, Value> const& m)
	{
		std::string tmp;
		tmp.append(serialize(static_cast<uint32_t>(m.size())));
		for (auto& p : m)
		{
			tmp.append(serialize(p));
		}
		return tmp;
	}

	template <typename Key, typename Value>
	inline int32_t deserialize(std::string& str, std::map<Key, Value>& m)
	{
		uint32_t size;
		int len = deserialize(str, size);
		str = str.substr(len);
		for (uint32_t i = 0; i < size; ++i)
		{
			std::pair<Key, Value> p;
			rola::deserialize(str, p);
			m[p.first] = std::move(p.second);

		}
		return 0;
	}

#pragma endregion

#pragma region serialize engine

	class object_serializer_helper
	{
		std::ostringstream oss_;

	public:
		object_serializer_helper()
			: oss_(std::ios::binary)
		{}

	public:

		template <typename SerializableType, typename = std::enable_if_t<std::is_fundamental_v<SerializableType>>>
		object_serializer_helper& operator<< (SerializableType a)
		{
			std::string x = serialize(a);
			oss_.write(x.data(), x.size());
			return *this;
		}

		template <typename SerializableType, typename = std::enable_if_t<!std::is_fundamental_v<SerializableType>>>
		object_serializer_helper& operator<< (SerializableType const& a)
		{
			std::string x = serialize(a);
			oss_.write(x.data(), x.size());
			return *this;
		}

		std::string str()
		{
			return oss_.str();
		}

		void empty()
		{
			oss_.str("");
		}
	};

	class object_deserializer_helper
	{
		std::string& str_;

	public:
		object_deserializer_helper(std::string& s)
			: str_(s)
		{}

		template<typename SerializableType>
		object_deserializer_helper& operator>> (SerializableType& a)
		{
			int ret = deserialize(str_, a);
			str_ = str_.substr(ret);
			return *this;
		}
	};

#pragma endregion
} // namespace rola

#endif // !ROLA_EASY_BUS_SERIALIZE_HPP
