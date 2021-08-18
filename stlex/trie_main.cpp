#include "trie.hpp"
#include <iostream>
#include <string>

int main_trie()
{
	rola::trie<std::string> t;
	t.insert({ "hi", "how", "are", "you" });
	t.insert({ "hi", "i", "am", "great", "thanks" });
	t.insert({ "what", "are", "you", "doing" });
	t.insert({ "i", "am", "watching", "a", "movie" });

	std::cout << "recorded sentences:\n";
	t.print(std::cout);

	std::cout << "\npossible suggestions after \"hi\":\n";
	auto st = t.subtrie(std::initializer_list<std::string>{"hi"});
	if (st)
	{
		st->get().print(std::cout);
	}

	std::cout << "trie_main successful\n";
	return 0;
}