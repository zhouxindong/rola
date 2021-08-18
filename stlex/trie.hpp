#ifndef ROLA_STLEX_TRIE_HPP
#define ROLA_STLEX_TRIE_HPP

#include <map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <ostream>
#include <optional>
#include <utility>

namespace rola
{
	template <typename T>
	class trie
	{
		std::map<T, trie> tries;

	public:
		template <typename It>
		void insert(It it, It end_it)
		{
			if (it == end_it)
			{
				return;
			}
			tries[*it].insert(std::next(it), end_it);
		}

		template <typename C>
		void insert(const C& container)
		{
			insert(std::begin(container), std::end(container));
		}

		void insert(const std::initializer_list<T>& il)
		{
			insert(std::begin(il), std::end(il));
		}

		void print(std::vector<T>& v, std::ostream& os) const
		{
			if (tries.empty())
			{
				std::copy(std::begin(v), std::end(v), std::ostream_iterator<T>{os, " "});
				os << '\n';
			}
			for (const auto& p : tries)
			{
				v.push_back(p.first);
				p.second.print(v, os);
				v.pop_back();
			}
		}

		void print(std::ostream& os) const
		{
			std::vector<T> v;
			print(v, os);
		}

		template <typename It>
		std::optional<std::reference_wrapper<const trie>>
			subtrie(It it, It end_it) const
		{
			if (it == end_it) return std::ref(*this);
			auto found(tries.find(*it));
			if (found == std::end(tries)) return {};
			return found->second.subtrie(std::next(it), end_it);
		}

		template <typename C>
		auto subtrie(const C& c)
		{
			return subtrie(std::begin(c), std::end(c));
		}
	};
} // namespace rola

#endif // !ROLA_STLEX_TRIE_HPP
