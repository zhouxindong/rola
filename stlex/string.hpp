#ifndef ROLA_STLEX_STRING_HPP
#define ROLA_STLEX_STRING_HPP

#include <vector>
#include <string>
#include <unordered_set>

namespace rola
{
	namespace stlex
	{
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

	} // namespace stlex
} // namespace rola

#endif // !ROLA_STLEX_STRING_HPP
