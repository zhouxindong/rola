#include <optional>

/// <summary>
/// takes a function f and return a new function
/// that essentially forwards calls to f but manipulates optionals and checks for failure
/// </summary>
/// <typeparam name="R"></typeparam>
/// <typeparam name="...P"></typeparam>
/// <param name="f"></param>
/// <returns></returns>
template <typename R, typename ... P>
auto make_failable(R(*f)(P ... ps))
{
	return [f](std::optional<P> ... xs)
		-> std::optional<R>
	{
		if ((xs && ...))
		{
			return { f(*(xs)...) };
		}
		else
		{
			return {};
		}
	}
}

int f1(int)
{
	return 0;
}

int f2(int, int)
{
	return 0;
}

int f3(int)
{
	return 0;
}

int f4(int)
{
	return 0;
}

auto failable_f1 = make_failable(f1);
auto failable_f2 = make_failable(f2);
auto failable_f3 = make_failable(f3);
auto failable_f4 = make_failable(f4);

failable_f4(failable_f4(failable_f3(
	failable_f2(failable_f1(x), failable_f1(y))
)))