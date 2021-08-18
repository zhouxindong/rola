#include <iostream>
#include <iomanip>
#include <thread>
#include <string>
#include <sstream>
#include <future>
#include <chrono>

struct pcout : public std::stringstream
{
	static inline std::mutex cout_mutex;

	~pcout()
	{
		std::lock_guard<std::mutex> l{ cout_mutex };
		std::cout << rdbuf();
		std::cout.flush();
	}
};

static std::string create(const char* s)
{
	pcout{} << "3s CREATE " << std::quoted(s) << '\n';
	std::this_thread::sleep_for(std::chrono::seconds(3));
	return { s };
}

static std::string concat(const std::string& a, const std::string& b)
{
	pcout{} << "5s CONCAT " << std::quoted(a) << " " << std::quoted(b) << '\n';
	std::this_thread::sleep_for(std::chrono::seconds(5));
	return a + b;
}

static std::string twice(const std::string& s)
{
	pcout{} << "3s TWICE " << std::quoted(s) << '\n';
	std::this_thread::sleep_for(std::chrono::seconds(3));
	return s + s;
}

/// <summary>
/// function can be execute async
/// </summary>
/// can be used
/// auto pcreate(asynchronize(func)) // pcreate is a callable captured the func #1
/// pcreate("foo") // not invoke, also a callable captured the arguments #2
/// so, operator() return a future object
/// <typeparam name="F"></typeparam>
/// <param name="f"></param>
/// <returns></returns>
template <typename F>
static auto asynchronize(F f)
{
	return [f](auto ... xs) // 1
	{
		return[=]()	// 2
		{
			return std::async(std::launch::async, f, xs...);
		};
	};
}

/// <summary>
/// return a callable can auto unwrap arguments used get() member function
/// </summary>
/// <typeparam name="F"></typeparam>
/// <param name="f"></param>
/// <returns></returns>
template <typename F>
static auto future_unwrap(F f)
{
	return [f](auto ... xs)
	{
		return f(xs.get()...);
	};
}

/// <summary>
/// makes a completely normal function wait for future argument and return a future result
/// it is kind of a translating wrapper from the synchronous to the async
/// </summary>
/// <typeparam name="F"></typeparam>
/// <param name="f"></param>
/// <returns></returns>
template <typename F>
static auto async_adapter(F f)
{
	return [f](auto ... xs)
	{
		return [=]()
		{
			return std::async(std::launch::async, future_unwrap(f), xs()...);
		};
	};
}

int main_auto_parall()
{
	auto pcreate(asynchronize(create));
	auto pconcat(async_adapter(concat));
	auto ptwice(async_adapter(twice));

	auto result(
		pconcat(
			ptwice(
				pconcat(
					pcreate("foo "),
					pcreate("bar ")
				)
			),
			pconcat(
				pcreate("this "),
				pcreate("that ")
			)
		)
	);

	std::cout << "Setup done. Nothing executed yet.\n";
	std::cout << result().get() << "\n";
	std::cout << "auto_parall.main.cpp successful\n";
	return 0;
}