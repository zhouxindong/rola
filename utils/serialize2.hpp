#ifndef ROLA_UTILS_SERIALIZE2_HPP
#define ROLA_UTILS_SERIALIZE2_HPP

#include <string>
#include <type_traits>
#include <sstream>
#include <vector>
#include <utility>
#include <map>

#include "utils/byte_order.hpp"

namespace rola
{
#pragma region type trait for serialize()|deserialize()

	namespace se_detail
	{
		/*
		* class member function std::string obj.serialize() const detect
		*/
		template <typename T, typename = std::string>
		struct has_serialize_member : std::false_type {};

		template <typename T>
		struct has_serialize_member<T, decltype(std::declval<const T&>().serialize())> : std::true_type {};

		/*
		* class member function T Class::deserialize(std::string&) detect
		*/
		template <typename T, typename = T>
		struct has_deserialize_member : std::false_type	{};

		template <typename T>
		struct has_deserialize_member<T, decltype(T::deserialize(std::declval<std::string&>()))>
			: std::true_type {};
	} // namespace se_detail

	template <typename T>
	inline std::enable_if_t<se_detail::has_serialize_member<T>::value, std::string>
		serialize(const T& obj)
	{
		return obj.serialize(); // serialize() const
	}

	template <typename T, typename = std::enable_if_t<se_detail::has_deserialize_member<T>::value>>
	inline T deserialize(std::string& s)
	{
		return T::deserialize(s);
	}

#pragma endregion
	
#pragma region inbuild type serialize

	namespace se_detail
	{
		/// <summary>
		/// no need consider byte order
		/// </summary>
		/// <typeparam name="T"></typeparam>
		template <typename T>
		struct is_normal_data : std::false_type {};

		template <>
		struct is_normal_data<float> : std::true_type {};

		template <>
		struct is_normal_data<double> : std::true_type {};

		template <>
		struct is_normal_data<int8_t> : std::true_type {};

		template <>
		struct is_normal_data<uint8_t> : std::true_type {};

		template <>
		struct is_normal_data<bool> : std::true_type {};

		template <>
		struct is_normal_data<char> : std::true_type {};

		/// <summary>
		/// need consider byte order
		/// </summary>
		/// <typeparam name="T"></typeparam>
		template <typename T>
		struct is_order_data : std::false_type {};

		template <>
		struct is_order_data<int16_t> : std::true_type {};

		template <>
		struct is_order_data<uint16_t> : std::true_type {};

		template <>
		struct is_order_data<int32_t> : std::true_type {};

		template <>
		struct is_order_data<uint32_t> : std::true_type {};

		template <>
		struct is_order_data<int64_t> : std::true_type {};

		template <>
		struct is_order_data<uint64_t> : std::true_type {};

		struct normal_data_helper
		{
			template <typename T>
			static std::string serialize(const T val)
			{
				std::string str;
				str.append((const char*)&val, sizeof(val));
				return str;
			}

			template <typename T>
			static T deserialize(std::string& s)
			{
				T ret{};
				::memcpy(&ret, s.data(), sizeof(T));
				s = s.substr(sizeof(T));
				return ret;
			}
		};

		struct order_data_helper
		{
			template <typename T>
			static std::string serialize(const T val)
			{
				std::string s;
				T n = host_to_network(val);
				s.append((const char*)(&n), sizeof(n));
				return s;
			}

			template <typename T>
			static T deserialize(std::string& s)
			{
				T ret{};
				::memcpy(&ret, s.data(), sizeof(T));
				ret = network_to_host(ret);
				s = s.substr(sizeof(T));
				return ret;
			}
		};

		template <
			typename T,
			bool IsNormal = is_normal_data<std::decay_t<T>>::value,
			bool IsOrder = is_order_data<std::decay_t<T>>::value>
			struct in_built_helper;

		template <typename T>
		struct in_built_helper<T, true, false> : normal_data_helper {};

		template <typename T>
		struct in_built_helper<T, false, true> : order_data_helper {};
	} // namespace se_detail

	template <typename T>
	inline std::string serialize(const T val)
	{
		return se_detail::in_built_helper<T>::serialize(val);
	}

	template <typename T>
	inline T deserialize(std::string& s)
	{
		return se_detail::in_built_helper<T>::deserialize<T>(s);
	}

#pragma endregion

#pragma region STL type serialize

	inline std::string serialize(const std::string& a)
	{
		int32_t len = static_cast<int32_t>(a.size());
		std::string ans;
		ans.append(serialize(len));
		ans.append(a);
		return ans;
	}

	inline std::string deserialize(std::string& str)
	{
		int32_t len = deserialize<int32_t>(str);
		std::string ret = str.substr(0, len);
		str = str.substr(len);
		return ret;
	}

	// build-in array
	template <typename T, uint32_t N>
	inline std::string serialize(T(&ary)[N])
	{
		std::string tmp;
		tmp.append(serialize<int32_t>(N));
		for (uint32_t i = 0; i < N; ++i)
		{
			tmp.append(serialize(ary[i]));
		}
		return tmp;
	}

	template <typename T, uint32_t N>
	inline void deserialize(std::string& str, T(&ary)[N])
	{
		int32_t size = deserialize<int32_t>(str);
		if (size != N)
			throw std::runtime_error("array size mismatched");

		for (int i = 0; i < size; ++i)
		{
			ary[i] = deserialize<T>(str);
		}
	}

	template <uint32_t N>
	inline void deserialize(std::string& str, std::string(&ary)[N])
	{
		int32_t size = deserialize<int32_t>(str);
		if (size != N)
			throw std::runtime_error("array size mismatched");

		for (int i = 0; i < size; ++i)
		{
			ary[i] = deserialize(str);
		}
	}
#pragma endregion
} // namespace rola

#endif // ROLA_UTILS_SERIALIZE2_HPP
