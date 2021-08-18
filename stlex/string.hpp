#ifndef ROLA_STLEX_STRING_HPP
#define ROLA_STLEX_STRING_HPP

#include <vector>
#include <string>
#include <unordered_set>
#include <sstream>
#include <locale>
#include <algorithm>

namespace rola
{
	template <typename CharT>
	using tstring = std::basic_string<CharT, std::char_traits<CharT>, std::allocator<CharT>>;

	template <typename CharT>
	using tstringstream = std::basic_string<CharT, std::char_traits<CharT>, std::allocator<CharT>>;

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

		inline std::string trim_whitespace_surrounding(const std::string& s)
		{
			const char whitespace[]{ " \t\n" };
			const size_t first(s.find_first_not_of(whitespace));
			if (std::string::npos == first)
				return {};

			const size_t last(s.find_last_not_of(whitespace));
			return s.substr(first, (last - first + 1));
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

		inline bool includes(std::string const& str, std::string const& sub_str)
		{
			return std::search(std::begin(str), std::end(str), std::begin(sub_str), std::end(sub_str))
				!= str.cend();
		}

		template <typename CharT>
		inline tstring<CharT> to_upper(tstring<CharT> text)
		{
			std::transform(std::begin(text), std::end(text),
				std::begin(text), toupper);
			return text;
		}

		template <typename CharT>
		inline tstring<CharT> to_lower(tstring<CharT> text)
		{
			std::transform(std::begin(text), std::end(text),
				std::begin(text), tolower);
			return text;
		}

		template <typename CharT>
		inline tstring<CharT> reverse(tstring<CharT> text)
		{
			std::reverse(std::begin(text), std::end(text));
			return text;
		}

		template <typename CharT>
		inline tstring<CharT> trim(tstring<CharT> const& text)
		{
			auto first{ text.find_first_not_of(' ') };
			auto last{ text.find_last_not_of(' ') };
			return text.substr(first, (last - first + 1));
		}

		template <typename CharT>
		inline tstring<CharT> trimleft(tstring<CharT> const& text)
		{
			auto first{ text.find_first_not_of(' ') };
			return text.substr(first, text.size() - first);
		}

		template <typename CharT>
		inline tstring<CharT> trimright(tstring<CharT> const& text)
		{
			auto last{ text.find_last_not_of(' ') };
			return text.substr(0, last + 1);
		}

		template <typename CharT>
		inline tstring<CharT> remove(tstring<CharT> text, CharT const ch)
		{
			auto start = std::remove_if(
				std::begin(text), std::end(text),
				[=](CharT const c) {return c == ch; }
			);
			text.erase(start, std::end(text));
			return text;
		}

		template <typename CharT>
		inline std::vector<tstring<CharT>> split(tstring<CharT> text, CharT const delimiter)
		{
			auto sstr = tstringstream<CharT>{ text };
			auto tokens = std::vector<tstring<CharT>>{};
			auto token = tstring<CharT>{};
			while (std::getline(sstr, token, delimiter))
			{
				if (!token.empty()) tokens.push_back(token);
			}
			return tokens;
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
