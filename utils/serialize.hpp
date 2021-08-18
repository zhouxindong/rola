#ifndef ROLA_UTILS_SERIALIZE_HPP
#define ROLA_UTILS_SERIALIZE_HPP

#include <string>
#include <type_traits>
#include <vector>
#include <sstream>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <tuple>

#include "utils/byte_order.hpp"

namespace rola
{

#pragma region public interfaces

	// 1. serialize_impl primary template, can be full or partial special
	template <typename T, typename U = void>
	struct serialize_impl;

	// 2. function template call for serialize|deserialize
	template <typename T>
	std::string serialize(const T& val)
	{
		return serialize_impl<T>::serialize(val);
	}

	template <typename T>
	T deserialize(std::string& s)
	{
		return serialize_impl<T>::deserialize(s);
	}

	// 2. special for array[]
	template <typename T, size_t N>
	std::string serialize(T(&ary)[N])
	{
		return serialize_impl<T[N]>::serialize(ary);
	}

	template <typename T, size_t N>
	void deserialize(std::string& s, T(&ary)[N])
	{
		serialize_impl<T[N]>::deserialize(s, ary);
	}

#pragma endregion

	// 3. full or partial special for special type

#pragma region 3.1 in-build type
	namespace se_detail
	{
		template <typename T>
		static std::string normal_serialize(const T val)
		{
			std::string str;
			str.append((const char*)&val, sizeof(val));
			return str;
		}

		template <typename T>
		static T normal_deserialize(std::string& s)
		{
			T ret{};
			::memcpy(&ret, s.data(), sizeof(T));
			s = s.substr(sizeof(T));
			return ret;
		}		

		template <typename T>
		static std::string order_serialize(const T val)
		{
			std::string s;
			T n = host_to_network(val);
			s.append((const char*)(&n), sizeof(n));
			return s;
		}

		template <typename T>
		static T order_deserialize(std::string& s)
		{
			T ret{};
			::memcpy(&ret, s.data(), sizeof(T));
			ret = network_to_host(ret);
			s = s.substr(sizeof(T));
			return ret;
		}
	} // namespace se_detail

	template <>
	struct serialize_impl<float>
	{
		static std::string serialize(const float val)
		{
			return se_detail::normal_serialize(val);
		}

		static float deserialize(std::string& s)
		{
			return se_detail::normal_deserialize<float>(s);
		}
	};

	template <>
	struct serialize_impl<double>
	{
		static std::string serialize(const double val)
		{
			return se_detail::normal_serialize(val);
		}

		static double deserialize(std::string& s)
		{
			return se_detail::normal_deserialize<double>(s);
		}
	};

	template <>
	struct serialize_impl<int8_t>
	{
		static std::string serialize(const int8_t val)
		{
			return se_detail::normal_serialize(val);
		}

		static int8_t deserialize(std::string& s)
		{
			return se_detail::normal_deserialize<int8_t>(s);
		}
	};

	template <>
	struct serialize_impl<uint8_t>
	{
		static std::string serialize(const uint8_t val)
		{
			return se_detail::normal_serialize(val);
		}

		static uint8_t deserialize(std::string& s)
		{
			return se_detail::normal_deserialize<uint8_t>(s);
		}
	};

	template <>
	struct serialize_impl<bool>
	{
		static std::string serialize(const bool val)
		{
			return se_detail::normal_serialize(val);
		}

		static bool deserialize(std::string& s)
		{
			return se_detail::normal_deserialize<bool>(s);
		}
	};

	template <>
	struct serialize_impl<char>
	{
		static std::string serialize(const char val)
		{
			return se_detail::normal_serialize(val);
		}

		static char deserialize(std::string& s)
		{
			return se_detail::normal_deserialize<char>(s);
		}
	};

	template <>
	struct serialize_impl<int16_t>
	{
		static std::string serialize(const int16_t val)
		{
			return se_detail::order_serialize(val);
		}

		static int16_t deserialize(std::string& s)
		{
			return se_detail::order_deserialize<int16_t>(s);
		}
	};

	template <>
	struct serialize_impl<uint16_t>
	{
		static std::string serialize(const uint16_t val)
		{
			return se_detail::order_serialize(val);
		}

		static uint16_t deserialize(std::string& s)
		{
			return se_detail::order_deserialize<uint16_t>(s);
		}
	};

	template <>
	struct serialize_impl<int32_t>
	{
		static std::string serialize(const int32_t val)
		{
			return se_detail::order_serialize(val);
		}

		static int32_t deserialize(std::string& s)
		{
			return se_detail::order_deserialize<int32_t>(s);
		}
	};

	template <>
	struct serialize_impl<uint32_t>
	{
		static std::string serialize(const uint32_t val)
		{
			return se_detail::order_serialize(val);
		}

		static uint32_t deserialize(std::string& s)
		{
			return se_detail::order_deserialize<uint32_t>(s);
		}
	};

	template <>
	struct serialize_impl<int64_t>
	{
		static std::string serialize(const int64_t val)
		{
			return se_detail::order_serialize(val);
		}

		static int64_t deserialize(std::string& s)
		{
			return se_detail::order_deserialize<int64_t>(s);
		}
	};

	template <>
	struct serialize_impl<uint64_t>
	{
		static std::string serialize(const uint64_t val)
		{
			return se_detail::order_serialize(val);
		}

		static uint64_t deserialize(std::string& s)
		{
			return se_detail::order_deserialize<uint64_t>(s);
		}
	};
#pragma endregion

#pragma region xx. helper functions

	namespace se_detail
	{
		template <typename T,
			typename = std::enable_if_t<std::is_class_v<T>>
		>
		std::string serialize_size(const T& cnt)
		{
			std::string tmp;
			int32_t len = static_cast<int32_t>(cnt.size());
			tmp.append(serialize_impl<int32_t>::serialize(len));
			return tmp;
		}

		template <typename T,
			typename = std::enable_if_t<std::is_integral_v<T>>>
		std::string serialize_size(const T size)
		{
			std::string tmp;
			int32_t len = static_cast<int32_t>(size);			
			tmp.append(serialize_impl<int32_t>::serialize(len));
			return tmp;
		}

		int32_t deserialize_size(std::string& str)
		{
			return serialize_impl<int32_t>::deserialize(str);
		}

		template <typename Cnt, typename = typename Cnt::value_type>
		std::string serialize_collections(const Cnt& c)
		{
			std::string tmp = serialize_size(c);
			for (const auto& item : c)
			{
				tmp.append(serialize_impl<typename Cnt::value_type>::serialize(item));
			}
			return tmp;
		}

		template <typename Cnt>
		Cnt deserialize_collections(std::string& s)
		{
			Cnt ret;

			int32_t size = se_detail::deserialize_size(s);
			for (auto i = 0; i < size; ++i)
			{
				ret.emplace(serialize_impl<typename Cnt::value_type>::deserialize(s));
			}
			return ret;
		}
	} // namespace se_detail

#pragma endregion

#pragma region 3.2 std::string

	template <>
	struct serialize_impl<std::string>
	{
		static std::string serialize(const std::string& a)
		{
			std::string ans = se_detail::serialize_size(a);
			ans.append(a);
			return ans;
		}

		static std::string deserialize(std::string& str)
		{
			int32_t len = se_detail::deserialize_size(str);
			std::string ret = str.substr(0, len);
			str = str.substr(len);
			return ret;
		}
	};

#pragma endregion

#pragma region 3.3 array[]

	template <typename T, size_t N>
	struct serialize_impl<T[N]>
	{
		static std::string serialize(T(&ary)[N])
		{
			std::string tmp = se_detail::serialize_size(N);
			for (size_t i = 0; i < N; ++i)
			{
				tmp.append(serialize_impl<T>::serialize(ary[i]));
			}
			return tmp;
		}

		static void deserialize(std::string& s, T(&ary)[N])
		{
			int32_t size = se_detail::deserialize_size(s);
			if (size != N)
				throw std::runtime_error("array size mismatched");

			for (int i = 0; i < size; ++i)
			{
				ary[i] = serialize_impl<T>::deserialize(s);
			}
		}
	};

#pragma endregion

#pragma region 3.4 std::array | std::vector

	template <typename T, size_t N>
	struct serialize_impl<std::array<T, N>>
	{
		static std::string serialize(const std::array<T, N>& ary)
		{
			return se_detail::serialize_collections(ary);
		}

		static std::array<T, N> deserialize(std::string& s)
		{
			int32_t size = se_detail::deserialize_size(s);
			if (size != N)
				throw std::runtime_error("std::array size mismatched");

			std::array<T, N> ary;
			for (int i = 0; i < size; ++i)
			{
				ary[i] = serialize_impl<T>::deserialize(s);
			}
			return ary;
		}
	};

	template <typename T>
	struct serialize_impl<std::vector<T>>
	{
		static std::string serialize(const std::vector<T>& v)
		{
			return se_detail::serialize_collections(v);
		}

		static std::vector<T> deserialize(std::string& s)
		{
			int32_t size = se_detail::deserialize_size(s);

			std::vector<T> v;
			v.reserve(size);
			for (int i = 0; i < size; ++i)
			{
				v.emplace_back(serialize_impl<T>::deserialize(s));
			}
			return v;
		}
	};

#pragma endregion

#pragma region 3.5 std::pair

	template <typename Key, typename Value>
	struct serialize_impl<std::pair<Key, Value>>
	{
		static std::string serialize(const std::pair<Key, Value>& val)
		{
			std::string tmp;
			tmp.append(serialize_impl<std::remove_const_t<Key>>::serialize(val.first));
			tmp.append(serialize_impl<Value>::serialize(val.second));
			return tmp;
		}

		static std::pair<Key, Value> deserialize(std::string& s)
		{
			auto key_val = serialize_impl<std::remove_const_t<Key>>::deserialize(s);
			auto value_val = serialize_impl<Value>::deserialize(s);
			std::pair<Key, Value> p{ key_val, value_val };
			return p;
		}
	};

#pragma endregion

#pragma region 3.6 class has special member functions

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
		struct has_deserialize_member : std::false_type {};

		template <typename T>
		struct has_deserialize_member<T, decltype(T::deserialize(std::declval<std::string&>()))>
			: std::true_type {};
	} // namespace se_detail

	template <typename T>
	struct serialize_impl<T,
		std::enable_if_t<se_detail::has_serialize_member<T>::value
		&& se_detail::has_deserialize_member<T>::value>>
	{
		static std::string serialize(const T& obj)
		{
			return obj.serialize();
		}

		static T deserialize(std::string& s)
		{
			return T::deserialize(s);
		}
	};

	class object_serializer_helper
	{
		std::ostringstream oss_{ std::ios::binary };

	public:
		template <typename T>
		object_serializer_helper& operator<<(const T& data)
		{
			auto sdata = rola::serialize(data);
			oss_ << sdata;
			return *this;
		}

		std::string str()
		{
			return oss_.str();
		}

		void clear()
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

		template<typename T>
		object_deserializer_helper& operator>> (T& a)
		{
			a = rola::deserialize<T>(str_);
			return *this;
		}
	};
#pragma endregion

#pragma region 3.7 std::map | std::set

	template <typename T>
	struct serialize_impl<std::set<T>>
	{
		static std::string serialize(const std::set<T>& se)
		{
			return se_detail::serialize_collections(se);
		}

		static std::set<T> deserialize(std::string& s)
		{
			return se_detail::deserialize_collections<std::set<T>>(s);
		}
	};

	template <typename T>
	struct serialize_impl<std::unordered_set<T>>
	{
		static std::string serialize(const std::unordered_set<T>& se)
		{
			return se_detail::serialize_collections(se);
		}

		static std::unordered_set<T> deserialize(std::string& s)
		{
			return se_detail::deserialize_collections<std::unordered_set<T>>(s);
		}
	};

	template <typename Key, typename Value>
	struct serialize_impl<std::map<Key, Value>>
	{
		static std::string serialize(const std::map<Key, Value>& m)
		{
			return se_detail::serialize_collections(m);
		}

		static std::map<Key, Value> deserialize(std::string& s)
		{
			return se_detail::deserialize_collections<std::map<Key, Value>>(s);
		}
	};

	template <typename Key, typename Value>
	struct serialize_impl<std::unordered_map<Key, Value>>
	{
		static std::string serialize(const std::unordered_map<Key, Value>& m)
		{
			return se_detail::serialize_collections(m);
		}

		static std::unordered_map<Key, Value> deserialize(std::string& s)
		{
			return se_detail::deserialize_collections<std::unordered_map<Key, Value>>(s);
		}
	};

#pragma endregion

#pragma region 3.8 std::tuple

	namespace se_detail
	{
		template <int Idx, int Max, typename ... Ts>
		struct tuple_serialize_helper
		{
			static void serialize_nth(const std::tuple<Ts...>& tup, std::string& s)
			{
				s.append(serialize(std::get<Idx>(tup)));
				tuple_serialize_helper<Idx + 1, Max, Ts...>::serialize_nth(tup, s);
			}

			template <typename Tup>
			static std::tuple<Ts...> deserialize_nth(std::string& s, Tup& tup)
			{
				using nth_type = std::tuple_element_t<Idx, std::tuple<Ts...>>;
				auto nth_value = serialize_impl<nth_type>::deserialize(s);
				return tuple_serialize_helper<Idx + 1, Max, Ts...>::deserialize_nth(s, std::tuple_cat(tup, std::tuple<nth_type>(nth_value)));
			}
		};

		template <int Max, typename ... Ts>
		struct tuple_serialize_helper<0, Max, Ts...>
		{
			static void serialize_nth(const std::tuple<Ts...>& tup, std::string& s)
			{
				s.append(serialize(std::get<0>(tup)));
				tuple_serialize_helper<1, Max, Ts...>::serialize_nth(tup, s);
			}

			static std::tuple<Ts...> deserialize_nth(std::string& s)
			{
				using first_type = std::tuple_element_t<0, std::tuple<Ts...>>;
				auto first_value =  serialize_impl<first_type>::deserialize(s);
				return tuple_serialize_helper<1, Max, Ts...>::deserialize_nth(s, std::tuple<first_type>(first_value));
			}
		};

		template <int Max, typename ... Ts>
		struct tuple_serialize_helper<Max, Max, Ts...>
		{
			static void serialize_nth(const std::tuple<Ts...>& tup, std::string& s)
			{
			}

			static std::tuple<Ts...> deserialize_nth(std::string& s, std::tuple<Ts...>& tup)
			{
				return tup;
			}
		};

	} // namespace se_detail

	template <typename ... Ts>
	struct serialize_impl<std::tuple<Ts...>>
	{
		static std::string serialize(const std::tuple<Ts...>& tup)
		{
			std::string tmp;
			se_detail::tuple_serialize_helper<0, sizeof...(Ts), Ts...>::serialize_nth(tup, tmp);
			return tmp;
		}

		static std::tuple<Ts...> deserialize(std::string& s)
		{
			return se_detail::tuple_serialize_helper<0, sizeof...(Ts), Ts...>::deserialize_nth(s);
		}
	};

#pragma endregion
} // namespace rola

#endif // ROLA_UTILS_SERIALIZE_HPP
