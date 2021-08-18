#include <tuple>
#include <iostream>
#include <string>
#include <utility>
#include <type_traits>
#include <algorithm>
#include <numeric>

// three ways to build a tuple
// 1. std::make_tuple (if you need values in the returned tuple)
// 2. std::tie        (if you need lvalue references in the returned tuple)
// 3. std::forward_as_tuple (if you if you need to keep the types of references of the inputs to build the tuple)

// error
template <typename... Ts>
class X
{
public:
	explicit X(Ts const&... values)
		: values_(std::make_tuple(values...))
	{}
	std::tuple<Ts const&...> values_;
};

// can be work
template <typename... Ts>
class X2
{
public:
	explicit X2(Ts const&... values)
		: values_(std::make_tuple(std::ref(values)...))
	{}
	std::tuple<Ts const&...> values_;
};

// correctly select
template <typename... Ts>
class X3
{
public:
	explicit X3(Ts const&... values)
		: values_(std::tie(values...))
	{}
	std::tuple<Ts const&...> values_;
};

std::string universe()
{
	return "universe";
}

int main_tuple()
{
	int i = 42;
	auto s = std::string("universe");

	// Exception!!
	//auto x = X<int, std::string>(i, s);
	//std::cout << "i = " << std::get<0>(x.values_) << "\n";
	//std::cout << "s = " << std::get<1>(x.values_) << "\n";

	auto x2 = X2<int, std::string>(i, s);
	std::cout << "i = " << std::get<0>(x2.values_) << "\n";
	std::cout << "s = " << std::get<1>(x2.values_) << "\n";

	auto x3 = X3<int, std::string>(i, s);
	std::cout << "i = " << std::get<0>(x3.values_) << "\n";
	std::cout << "s = " << std::get<1>(x3.values_) << "\n";

	auto myTuple = std::forward_as_tuple(i, universe()); // <int&, std::string&&>
	static_assert(std::is_same_v<decltype(myTuple),
		std::tuple<int&, std::string&&>>);

	std::cout << "tuple_main successful\n";
	return 0;
}

static void print_student(size_t id, const std::string& name, double gpa)
{
	std::cout << "Student " << name
		<< ", ID: " << id
		<< ", GPA: " << gpa << "\n";
}

void main_tuple2()
{
	using student = std::tuple<size_t, std::string, double>;
	student john{ 123, "Hoh", 3.7 };
	const auto& [id, name, gpa] = john;
	print_student(id, name, gpa);
	std::apply(print_student, john);
}

template <typename T, typename ... Ts>
void print_args(std::ostream& os, const T& v, const Ts& ... vs)
{
	os << v;
	(void)std::initializer_list<int>{((os << ", " << vs), 0)...};
}

template <typename ... Ts>
std::ostream& operator<<(std::ostream& os, const std::tuple<Ts...>& t)
{
	auto print_to_os([&os](const auto& ... xs) {
		print_args(os, xs...);
		});
	os << "(";
	std::apply(print_to_os, t);
	return os << ")";
}

int main_tuple3()
{
	print_args(std::cout, 1, 2, "foo", 3, "bar");
	std::cout << "\ntuple_main.cpp successful\n";
	return 0;
}

template <typename T>
std::tuple<double, double, double, double>
sum_min_max_avg(const T& range)
{
	auto min_max(std::minmax_element(std::begin(range), std::end(range)));
	auto sum(std::accumulate(std::begin(range), std::end(range), 0.0));
	return { sum, *min_max.first, *min_max.second, sum / range.size() };
}

template <typename T1, typename T2>
static auto zip(const T1& a, const T2& b)
{
	auto z([](auto ...xs) {
		return[xs...](auto ...ys)
		{
			return std::tuple_cat(std::make_tuple(xs, ys)...);
		};
		});

	return std::apply(std::apply(z, a), b);
}

int main_tuple4()
{
	auto student_desc(std::make_tuple("ID", "Name", "GPA"));
	auto student(std::make_tuple(123456, "John Doe", 3.7));

	std::cout << student_desc << '\n'
		<< student << '\n';

	std::cout << std::tuple_cat(student_desc, student) << '\n';

	auto zipped(zip(student_desc, student));
	std::cout << zipped << '\n';

	auto numbers = { 0.0, 1.0, 2.0, 3.0, 4.0 };
	std::cout << zip(std::make_tuple("Sum", "Minimum", "Maximum", "Average"),
		sum_min_max_avg(numbers)) << '\n';

	std::cout << "tuple_main.cpp successful\n";
	return 0;

}