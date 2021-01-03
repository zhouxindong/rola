#ifndef ROLA_STLEX_STRING_HPP
#define ROLA_STLEX_STRING_HPP

#include <vector>
#include <string>
#include <unordered_set>
#include <sstream>
#include <locale>

namespace rola
{
	namespace stlex
	{
#pragma region string

		inline std::vector<std::string> split(const std::string& text, const std::unordered_set<char>& tag)
		{
			std::vector<std::string> ret_vec;
			std::string sub_str;

			for (size_t i = 0; i < text.length(); i++)
			{
				if (tag.find(text[i]) != tag.cend())
				{
					if (!sub_str.empty())
					{
						ret_vec.push_back(sub_str);
						sub_str.clear();
					}
				}
				else
				{
					sub_str.push_back(text[i]);
				}
			}

			if (!sub_str.empty())
			{
				ret_vec.push_back(sub_str);
			}

			return ret_vec;
		}

		inline bool is_not_space(char c)
		{
			return c != ' ';
		}

		inline std::string trim_left(std::string s)
		{
			s.erase(s.begin(),
				std::find_if(s.begin(), s.end(), is_not_space));
			return s;
		}

		inline std::string trim_right(std::string s)
		{
			s.erase(std::find_if(s.rbegin(), s.rend(), is_not_space).base(), s.end());
			return s;
		}

		inline std::string trim(std::string s)
		{
			return trim_left(trim_right(std::move(s)));
		}

		template <typename T>
		std::string to_string(T const& v)
		{
			std::ostringstream oss;
			oss << v;
			return oss.str();
		}

		template <size_t N>
		std::string frombytes(const char(&ary)[N])
		{
			std::vector<char> v;
			for (int i = 0; i < N; ++i)
			{
				if (ary[i] != '\0')
					v.push_back(ary[i]);
			}

			return std::string(v.begin(), v.end());
		}

#pragma endregion

#pragma region ostringstream

		inline std::ostringstream& clear_content(std::ostringstream& oss)
		{
			oss.str("");
			return oss;
		}

#pragma endregion
	} // namespace stlex
} // namespace rola

#endif // !ROLA_STLEX_STRING_HPP
