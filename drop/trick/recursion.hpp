
#include <vector>
#include <string>

template <typename FilterFunction, typename Iterator>
std::vector<std::string> names_for_helper(
	Iterator people_begin,
	Iterator people_end,
	FilterFunction filter,
	std::vector<std::string> previously_collected)
{
	if (people_begin == people_end)
	{
		return previously_collected;
	}
	else
	{
		const auto head = *people_begin;
		if (filter(head))
		{
			previously_collected.push_back(name(head));
		}
		return names_for_helper(
			people_begin + 1,
			people_end,
			filter,
			std::move(previously_collected)
		);
	}
}

template <typename FilterFunction, typename Iterator>
std::vector<std::string> names_for(
	Iterator people_begin,
	Iterator people_end,
	FilterFunction filter)
{
	return names_for_helper(people_begin, people_end, filter, {});
}