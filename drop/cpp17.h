
// explicit deduction guide
#include <type_traits>
#include <algorithm>
#include <vector>

template <typename T>
struct sum
{
	T value;

	template <typename ... Ts>
	sum(Ts&& ... values) : value{ (values + ...) }
	{}
};

template <typename ... Ts>
sum(Ts&& ... ts) -> sum<std::common_type_t<Ts...>>;

// fold
template <typename ... Ts>
auto sum(Ts ... ts)
{
	return (ts + ... + 0); // right fold
	return (0 + ... + ts); // left fold
}

template <typename R, typename ... Ts>
auto matches(const R& range, Ts ... ts)
{
	return (std::count(std::begin(range), std::end(range), ts) + ...);
}

template <typename T, typename ... Ts>
void insert_all(std::vector<T>& vec, Ts ... ts)
{
	(vec.push_back(ts), ...); // void
}