
#include <iostream>
#include <vector>
#include <functional>
#include <optional>
#include <utility>
#include <string>
#include <ostream>

template <typename T>
std::ostream& operator<<(std::ostream& out, std::vector<T> const& v)
{
	out << "size: " << v.size() << std::endl;
	out << "elements:";
	for (const T& item : v)
	{
		out << " " << item;
	}

	return out;
}

template <typename T>
std::ostream& operator<<(std::ostream& out, std::vector<std::optional<T>> const& v)
{
	out << "size: " << v.size() << std::endl;
	out << "elements:";
	for (const auto& item : v)
	{
		out << " " << *item;
	}

	return out;
}

template <typename T, typename U>
std::vector<U> map(const std::function<U(T)>& f,
	const std::vector<T>& a)
{
	std::vector<U> result;
	for (const T& t : a)
		result.push_back(f(t));
	return result;
}

template <typename T, typename U>
std::optional<U> map(std::function<U(T)> f, std::optional<T> a)
{
	if (a)
		return std::optional<U>(f(*a));
	else
		return std::nullopt;
}

template <typename A, typename T, typename U>
std::pair<A, U> map(std::function<U(T)> f, std::pair<A, T> a)
{
	return std::make_pair(a.first, f(a.second));
}

template <typename P, typename T, typename U>
std::function<U(P)> map(std::function<U(T)> f, std::function<T(P)> a)
{
	return [f, a](P p)
	{
		return f(a(p));
	};
}

template <typename T>
std::optional<T> pure(T t)
{
	return std::optional<T>(t);
}

template <typename T, typename U>
std::optional<U> apply(std::optional<std::function<U(T)>> a, std::optional<T> b)
{
	if (a && b)
		return (*a)(*b);
	else
		return std::nullopt;
}

template <typename T>
std::optional<T> join(std::optional<std::optional<T>> a)
{
	if (a)
		return *a;
	else
		return std::nullopt;
}

template <typename T>
std::vector<T> join(std::vector<std::vector<T>> vec)
{
	std::vector<T> ret;
	for (auto&& v : vec)
	{
		std::move(v.begin(), v.end(), std::back_inserter(ret));
	}
	return ret;
}

std::vector<std::optional<std::string>> get_strings(std::vector<int> const& v)
{
	auto f1 = [](auto i) {return std::to_string(i); }; // int -> std::string
	auto f2 = [](auto s) {return std::optional<std::string>(s); }; // std::string -> std::optional<std::string>
	auto f3 = [f1, f2](auto i) {return map<int, std::string, std::optional<std::string>>(f2, f1)(i); }; // int -> std::optional<std::string>

	return map<int, std::optional<std::string>>(f3, v); // vector<int> -> vector<std::optional<std::string>>
}

int main_monads()
{
	std::vector<int> v{ 1,2,3,4,5 };
	std::cout << get_strings(v);

	//const std::optional<double> a = /* etc. */;
	//const std::optional<double> b = /* etc. */;
	//const std::optional<double> c = apply(pure(std::plus<double>()), a, b);
	//const std::optional<double> d = apply(pure(std::negate<double>()), c);

	std::cout << "\nmonads.cpp successful\n";
	return 0;
}